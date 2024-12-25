use serde::ser::SerializeSeq;
use serde::{Serialize, Serializer};
use sqlx::{query, query_as, FromRow, Postgres, Transaction};
use time::PrimitiveDateTime;

use crate::error::{BackendError, BackendErrorKind, BackendErrorTemplate};
use crate::utils::time::{
    get_current_primitive_date_time, serialize_primitive_date_time_as_unix_timestamp,
};
use crate::{db, snowflake_generator};

// TODO: remove, when rmp-serde will support `flatten` serde attribute
pub fn serialize_booking<S>(booking: &Booking, serializer: S) -> Result<S::Ok, S::Error>
where
    S: Serializer,
{
    let mut sequence = serializer.serialize_seq(Some(6))?;

    sequence.serialize_element(&booking.id)?;
    sequence.serialize_element(&booking.hotel_id)?;
    sequence.serialize_element(&booking.room_id)?;
    sequence.serialize_element(&booking.check_in_at)?;
    sequence.serialize_element(&booking.check_out_at)?;
    sequence.serialize_element(&booking.created_at)?;

    sequence.end()
}

#[derive(Clone, Serialize)]
pub struct BookingWithGuestIds {
    #[serde(serialize_with = "serialize_booking")]
    pub booking: Booking,
    pub guest_ids: Vec<i64>,
}

#[derive(Clone, FromRow, Serialize)]
#[allow(dead_code)]
pub struct Booking {
    pub id: i64,
    pub hotel_id: i64,
    pub room_id: i64,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub check_in_at: PrimitiveDateTime,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub check_out_at: PrimitiveDateTime,
    #[serde(serialize_with = "serialize_primitive_date_time_as_unix_timestamp")]
    pub created_at: PrimitiveDateTime,
}

impl Booking {
    // CRUD
    pub async fn create(
        hotel_id: i64,
        room_id: i64,
        check_in_at: PrimitiveDateTime,
        check_out_at: PrimitiveDateTime,
        guest_ids: Vec<i64>,
    ) -> Result<BookingWithGuestIds, BackendError> {
        let connection = db::get_connection();
        let mut transaction = connection.begin().await?;

        let booking = query_as!(
            Self,
            r#"
                INSERT INTO bookings (id, hotel_id, room_id, check_in_at, check_out_at)
                VALUES ($1, $2, $3, $4, $5)
                RETURNING *
            "#,
            snowflake_generator::generate(),
            hotel_id,
            room_id,
            check_in_at,
            check_out_at,
        )
        .fetch_one(&mut *transaction)
        .await?;

        for guest_id in guest_ids.clone() {
            BookingGuest::create(booking.id, guest_id, Some(&mut transaction)).await?;
        }

        transaction.commit().await?;

        Ok(BookingWithGuestIds { booking, guest_ids })
    }

    // Default implementation
    pub async fn check_availability(
        hotel_id: &i64,
        room_id: &i64,
        check_in_at: &PrimitiveDateTime,
        check_out_at: &PrimitiveDateTime,
    ) -> Result<(), BackendError> {
        let connection = db::get_connection();

        // TODO: check-in guests at the same day, when another guest checks-out
        let bookings = query!(
            r#"
                SELECT COUNT(*)
                FROM bookings
                WHERE hotel_id = $1
                  AND room_id = $2
                  AND check_in_at <= $3
                  AND check_out_at >= $4
            "#,
            hotel_id,
            room_id,
            check_in_at,
            check_out_at,
        )
        .fetch_one(connection)
        .await?
        .count;

        match bookings {
            Some(bookings) => {
                if bookings > 0 {
                    return Err(BackendErrorTemplate::BookingRoomNotAvailable.into());
                }
            }
            None => {
                return Err(BackendError::new_internal(
                    "Failed to check room availability, because count is None".into(),
                    BackendErrorKind::Other(Some("Option is None".into())),
                ))
            }
        }

        Ok(())
    }

    pub fn check_check_in_at_and_check_out_at(
        check_in_at: &PrimitiveDateTime,
        check_out_at: &PrimitiveDateTime,
    ) -> Result<(), BackendError> {
        if check_in_at < &get_current_primitive_date_time() {
            return Err(BackendErrorTemplate::BookingPastCheckInAt.into());
        }
        if check_out_at < &get_current_primitive_date_time() {
            return Err(BackendErrorTemplate::BookingPastCheckOutAt.into());
        }
        if check_in_at > check_out_at {
            return Err(BackendErrorTemplate::BookingCheckInAtAfterCheckOutAt.into());
        }

        Ok(())
    }
}

// Maybe I should move it to a separate service
#[derive(Clone, FromRow, Serialize)]
#[allow(dead_code)]
pub struct BookingGuest {
    pub id: i64,
    pub booking_id: i64,
    pub guest_id: i64,
}

impl BookingGuest {
    pub async fn create(
        booking_id: i64,
        guest_id: i64,
        transaction: Option<&mut Transaction<'_, Postgres>>,
    ) -> Result<Self, BackendError> {
        let query = query_as!(
            Self,
            r#"
                INSERT INTO booking_guests (id, booking_id, guest_id)
                VALUES ($1, $2, $3)
                RETURNING *
            "#,
            snowflake_generator::generate(),
            booking_id,
            guest_id,
        );

        let booking_guest = match transaction {
            Some(transaction) => query.fetch_one(&mut **transaction).await?,
            None => {
                let connection = db::get_connection();
                query.fetch_one(connection).await?
            }
        };

        Ok(booking_guest)
    }
}
