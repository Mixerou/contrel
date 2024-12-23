use serde::Serialize;
use sqlx::{query_as, FromRow};
use time::PrimitiveDateTime;

use crate::error::{BackendError, BackendErrorTemplate};
use crate::utils::time::serialize_primitive_date_time_as_unix_timestamp;
use crate::{db, snowflake_generator};

#[derive(Clone, FromRow, Serialize)]
#[allow(dead_code)]
pub struct Room {
    pub id: i64,
    pub number: String,
    pub group_name: String,
    pub hotel_id: i64,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub created_at: PrimitiveDateTime,
}

impl Room {
    // CRUD
    pub async fn create(
        number: String,
        group_name: String,
        hotel_id: i64,
    ) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let room = query_as!(
            Self,
            r#"
                INSERT INTO rooms (id, number, group_name, hotel_id)
                VALUES ($1, $2, $3, $4)
                RETURNING *
            "#,
            snowflake_generator::generate(),
            number,
            group_name,
            hotel_id,
        )
        .fetch_one(connection)
        .await?;

        Ok(room)
    }

    pub async fn find_all_by_hotel_id(hotel_id: &i64) -> Result<Vec<Self>, BackendError> {
        let connection = db::get_connection();

        let rooms = query_as!(
            Self,
            r#"
                SELECT *
                FROM rooms
                WHERE hotel_id = $1
            "#,
            hotel_id,
        )
        .fetch_all(connection)
        .await?;

        Ok(rooms)
    }

    // Default implementations
    pub fn check_number(number: &str) -> Result<(), BackendError> {
        let number_length = number.chars().count();

        match number_length {
            number if number < 1 => return Err(BackendErrorTemplate::RoomNumberTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::RoomNumberTooLong.into()),
            _ => {}
        };

        Ok(())
    }

    pub fn check_group_name(group_name: &str) -> Result<(), BackendError> {
        let group_name_length = group_name.chars().count();

        match group_name_length {
            number if number < 3 => return Err(BackendErrorTemplate::RoomGroupNameTooShort.into()),
            number if number > 32 => return Err(BackendErrorTemplate::RoomGroupNameTooLong.into()),
            _ => {}
        };

        Ok(())
    }
}
