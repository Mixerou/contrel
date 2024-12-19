use serde::Serialize;
use serde_repr::{Deserialize_repr, Serialize_repr};
use sqlx::{query_as, FromRow, Type};
use time::PrimitiveDateTime;

use crate::error::BackendError;
use crate::types::{Country, Gender};
use crate::utils::time::serialize_primitive_date_time_as_unix_timestamp;
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
}

#[derive(Clone, Eq, PartialEq, Hash, Deserialize_repr, Serialize_repr, Type)]
#[repr(i16)]
pub enum DocumentType {
    Passport,
}
