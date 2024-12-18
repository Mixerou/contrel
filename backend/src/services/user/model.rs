use serde::Serialize;
use sqlx::{query_as, FromRow};
use time::PrimitiveDateTime;

use crate::error::{BackendError, BackendErrorTemplate};
use crate::utils::time::serialize_primitive_date_time_as_unix_timestamp;
use crate::{db, snowflake_generator};

#[derive(Clone, FromRow, Serialize)]
#[allow(dead_code)]
pub struct User {
    pub id: i64,
    pub email: String,
    #[serde(skip)]
    pub password_hash: String,
    pub first_name: String,
    pub last_name: String,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub created_at: PrimitiveDateTime,
}

impl User {
    // CRUD
    pub async fn create(
        email: String,
        password_hash: String,
        first_name: String,
        last_name: String,
    ) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let user = query_as!(
            Self,
            r#"
                INSERT INTO users (id, email, password_hash, first_name, last_name)
                VALUES ($1, $2, $3, $4, $5)
                RETURNING *
            "#,
            snowflake_generator::generate(),
            email,
            password_hash,
            first_name,
            last_name,
        )
        .fetch_one(connection)
        .await?;

        Ok(user)
    }

    pub async fn find(id: &i64) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let user = query_as!(
            Self,
            r#"
                SELECT *
                FROM users
                WHERE id = $1
            "#,
            id,
        )
        .fetch_one(connection)
        .await?;

        Ok(user)
    }

    pub async fn find_by_email(email: &str) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let user = query_as!(
            Self,
            r#"
                SELECT *
                FROM users
                WHERE email = $1
            "#,
            email,
        )
        .fetch_one(connection)
        .await?;

        Ok(user)
    }

    // Default implementations
    pub fn check_email(email: &str) -> Result<(), BackendError> {
        let email_length = email.chars().count();

        match email_length {
            number if number < 6 => return Err(BackendErrorTemplate::UserEmailTooShort.into()),
            number if number > 64 => return Err(BackendErrorTemplate::UserEmailTooLong.into()),
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

    pub fn check_password_length(password: &str) -> Result<(), BackendError> {
        let password_length = password.chars().count();

        match password_length {
            number if number < 8 => Err(BackendErrorTemplate::UserPasswordTooShort.into()),
            number if number > 128 => Err(BackendErrorTemplate::UserPasswordTooLong.into()),
            _ => Ok(()),
        }
    }

    pub fn check_first_name(first_name: &str) -> Result<(), BackendError> {
        let first_name_length = first_name.chars().count();

        match first_name_length {
            number if number < 3 => return Err(BackendErrorTemplate::FirstNameTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::FirstNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }

    pub fn check_last_name(last_name: &str) -> Result<(), BackendError> {
        let last_name_length = last_name.chars().count();

        match last_name_length {
            number if number < 3 => return Err(BackendErrorTemplate::LastNameTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::LastNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }
}
