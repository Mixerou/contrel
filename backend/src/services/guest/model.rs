use serde::Serialize;
use serde_repr::{Deserialize_repr, Serialize_repr};
use sqlx::{query_as, FromRow, Type};
use time::PrimitiveDateTime;

use crate::error::{BackendError, BackendErrorTemplate};
use crate::types::{Country, Gender};
use crate::utils::time::{
    get_current_primitive_date_time, serialize_primitive_date_time_as_unix_timestamp,
};
use crate::{db, snowflake_generator};

#[derive(Clone, FromRow, Serialize)]
pub struct Guest {
    pub id: i64,
    pub first_name: String,
    pub last_name: String,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub date_of_birth: PrimitiveDateTime,
    pub gender: Gender,
    pub phone_number: String,
    pub email: String,
    pub document_type: DocumentType,
    pub document_number: String,
    pub document_country: Country,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub document_valid_until: PrimitiveDateTime,
    pub notes: String,
    pub hotel_id: i64,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub created_at: PrimitiveDateTime,
}

impl Guest {
    pub async fn create(
        first_name: String,
        last_name: String,
        date_of_birth: PrimitiveDateTime,
        gender: Gender,
        phone_number: String,
        email: String,
        document_type: DocumentType,
        document_number: String,
        document_country: Country,
        document_valid_until: PrimitiveDateTime,
        notes: String,
        hotel_id: i64,
    ) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let user = query_as!(
            Self,
            r#"
                INSERT INTO guests (id,
                        first_name,
                        last_name,
                        date_of_birth,
                        gender,
                        phone_number,
                        email,
                        document_type,
                        document_number,
                        document_country,
                        document_valid_until,
                        notes,
                        hotel_id)
                VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13)
                RETURNING id,
                    first_name,
                    last_name,
                    date_of_birth,
                    gender AS "gender: Gender",
                    phone_number,
                    email,
                    document_type AS "document_type: DocumentType",
                    document_number,
                    document_country AS "document_country: Country",
                    document_valid_until,
                    notes,
                    hotel_id,
                    created_at
            "#,
            snowflake_generator::generate(),
            first_name,
            last_name,
            date_of_birth,
            gender as Gender,
            phone_number,
            email,
            document_type as DocumentType,
            document_number,
            document_country as Country,
            document_valid_until,
            notes,
            hotel_id,
        )
        .fetch_one(connection)
        .await?;

        Ok(user)
    }

    pub async fn find_all_by_hotel_id(hotel_id: &i64) -> Result<Vec<Self>, BackendError> {
        let connection = db::get_connection();

        let guests = query_as!(
            Self,
            r#"
                SELECT id,
                       first_name,
                       last_name,
                       date_of_birth,
                       gender AS "gender: Gender",
                       phone_number,
                       email,
                       document_type AS "document_type: DocumentType",
                       document_number,
                       document_country AS "document_country: Country",
                       document_valid_until,
                       notes,
                       hotel_id,
                       created_at
                FROM guests
                WHERE hotel_id = $1
            "#,
            hotel_id,
        )
        .fetch_all(connection)
        .await?;

        Ok(guests)
    }

    // Default implementations
    pub fn check_first_name(first_name: &str) -> Result<(), BackendError> {
        let first_name_length = first_name.chars().count();

        match first_name_length {
            number if number < 3 => return Err(BackendErrorTemplate::GuestFirstNameTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::GuestFirstNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }

    pub fn check_last_name(last_name: &str) -> Result<(), BackendError> {
        let last_name_length = last_name.chars().count();

        match last_name_length {
            number if number < 3 => return Err(BackendErrorTemplate::GuestLastNameTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::GuestLastNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }

    pub fn check_date_of_birth(date_of_birth: &PrimitiveDateTime) -> Result<(), BackendError> {
        if date_of_birth > &get_current_primitive_date_time() {
            return Err(BackendErrorTemplate::FutureDateOfBirth.into());
        }

        Ok(())
    }

    pub fn check_phone_number(phone_number: &str) -> Result<(), BackendError> {
        let phone_number: Vec<char> = phone_number.chars().collect();

        match phone_number[0] {
            '+' | '0'..='9' => {}
            _ => return Err(BackendErrorTemplate::BadPhoneNumber.into()),
        }

        for char in phone_number[1..].iter() {
            if !char.is_ascii_digit() {
                return Err(BackendErrorTemplate::BadPhoneNumber.into());
            }
        }

        match phone_number.len() {
            number if number < 8 => {
                return Err(BackendErrorTemplate::GuestPhoneNumberTooShort.into())
            }
            number if number > 16 => {
                return Err(BackendErrorTemplate::GuestPhoneNumberTooLong.into())
            }
            _ => {}
        };

        Ok(())
    }

    pub fn check_email(email: &str) -> Result<(), BackendError> {
        let email_length = email.chars().count();

        match email_length {
            number if number < 6 => return Err(BackendErrorTemplate::GuestEmailTooShort.into()),
            number if number > 64 => return Err(BackendErrorTemplate::GuestEmailTooLong.into()),
            _ => {}
        }

        let parts = email.split('@').collect::<Vec<&str>>();

        if parts.len() != 2
            || parts.first().unwrap_or(&"").is_empty()
            || parts.last().unwrap_or(&"").is_empty()
        {
            return Err(BackendErrorTemplate::BadEmail.into());
        }

        Ok(())

        // PS: I don't want for this project to make deep email check :)
    }

    pub fn check_document_number(document_number: &str) -> Result<(), BackendError> {
        let document_number_length = document_number.chars().count();

        match document_number_length {
            number if number < 6 => {
                return Err(BackendErrorTemplate::GuestDocumentNumberTooShort.into())
            }
            number if number > 16 => {
                return Err(BackendErrorTemplate::GuestDocumentNumberTooLong.into())
            }
            _ => {}
        };

        Ok(())
    }

    pub fn check_document_valid_until(
        document_valid_until: &PrimitiveDateTime,
    ) -> Result<(), BackendError> {
        if document_valid_until < &get_current_primitive_date_time() {
            return Err(BackendErrorTemplate::GuestPastDocumentValidUntil.into());
        }

        Ok(())
    }

    pub fn check_notes(notes: &str) -> Result<(), BackendError> {
        let notes_length = notes.chars().count();

        if notes_length > 128 {
            return Err(BackendErrorTemplate::GuestNotesTooLong.into());
        }

        Ok(())
    }
}

#[derive(Clone, Eq, PartialEq, Hash, Deserialize_repr, Serialize_repr, Type)]
#[repr(i16)]
pub enum DocumentType {
    Passport,
}
