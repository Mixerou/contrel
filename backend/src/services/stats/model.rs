use serde::Serialize;
use sqlx::{query_as, FromRow};
use time::PrimitiveDateTime;

use crate::db;
use crate::error::BackendError;
use crate::utils::time::serialize_primitive_date_time_as_unix_timestamp;

#[derive(Debug, FromRow, Serialize)]
pub struct StatsLongTimeGuest {
    pub guest_id: i64,
    pub hotel_id: i64,
    pub room_id: i64,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub check_in_at: PrimitiveDateTime,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub check_out_at: PrimitiveDateTime,
}

impl StatsLongTimeGuest {
    pub async fn find_all_by_hotel_id(hotel_id: &i64) -> Result<Vec<Self>, BackendError> {
        let connection = db::get_connection();

        // TODO: Maybe someday interval will not be hardcoded
        let guests = query_as!(
            Self,
            r#"
                SELECT booking_guests.guest_id,
                       bookings.hotel_id,
                       bookings.room_id,
                       bookings.check_in_at,
                       bookings.check_out_at
                FROM booking_guests
                         LEFT JOIN bookings ON booking_guests.booking_id = bookings.id
                WHERE bookings.hotel_id = $1
                  AND bookings.check_in_at <= current_timestamp
                  AND bookings.check_out_at > current_timestamp
                  AND EXTRACT(DAY FROM current_timestamp - bookings.check_in_at) > 10;
            "#,
            hotel_id,
        )
        .fetch_all(connection)
        .await?;

        Ok(guests)
    }
}
