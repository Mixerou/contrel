use serde::Serialize;
use sqlx::{query_as, FromRow};
use time::PrimitiveDateTime;

use crate::error::{BackendError, BackendErrorTemplate};
use crate::utils::time::serialize_primitive_date_time_as_unix_timestamp;
use crate::{db, snowflake_generator};

#[derive(Clone, FromRow, Serialize)]
#[allow(dead_code)]
pub struct Hotel {
    pub id: i64,
    pub name: String,
    pub owner_id: i64,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub created_at: PrimitiveDateTime,
}

impl Hotel {
    // CRUD
    pub async fn create(name: String, owner_id: i64) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let user = query_as!(
            Self,
            r#"
                INSERT INTO hotels (id, name, owner_id)
                VALUES ($1, $2, $3)
                RETURNING *
            "#,
            snowflake_generator::generate(),
            name,
            owner_id,
        )
        .fetch_one(connection)
        .await?;

        Ok(user)
    }

    pub async fn find(id: &i64) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let hotel = query_as!(
            Self,
            r#"
                SELECT *
                FROM hotels
                WHERE id = $1
            "#,
            id,
        )
        .fetch_one(connection)
        .await?;

        Ok(hotel)
    }

    pub async fn find_all_by_owner_id(owner_id: &i64) -> Result<Vec<Self>, BackendError> {
        let connection = db::get_connection();

        let hotels = query_as!(
            Self,
            r#"
                SELECT *
                FROM hotels
                WHERE owner_id = $1
            "#,
            owner_id,
        )
        .fetch_all(connection)
        .await?;

        Ok(hotels)
    }

    // Default implementations
    pub fn check_name(name: &str) -> Result<(), BackendError> {
        let name_length = name.chars().count();

        match name_length {
            number if number < 3 => return Err(BackendErrorTemplate::HotelNameTooShort.into()),
            number if number > 64 => return Err(BackendErrorTemplate::HotelNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }
}
