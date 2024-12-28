use actix_web::web::{Bytes, Path, ReqData, ServiceConfig};
use actix_web::{get, post, HttpResponse};
use serde::Deserialize;
use time::PrimitiveDateTime;

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::booking::Booking;
use crate::services::hotel::Hotel;
use crate::services::room::Room;
use crate::services::session::Session;
use crate::utils::time::deserialize_from_unix_timestamp;

#[derive(Deserialize)]
struct BookingCreationPath {
    hotel_id: i64,
}

#[derive(Deserialize)]
struct BookingCreationRequest {
    room_id: i64,
    #[serde(deserialize_with = "deserialize_from_unix_timestamp")]
    check_in_at: PrimitiveDateTime,
    #[serde(deserialize_with = "deserialize_from_unix_timestamp")]
    check_out_at: PrimitiveDateTime,
    guest_ids: Vec<i64>,
}

#[post("/hotels/{hotel_id}/bookings")]
async fn create(
    session: ReqData<Session>,
    path: Path<BookingCreationPath>,
    payload: Bytes,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let Ok(request) = rmp_serde::from_slice::<BookingCreationRequest>(payload.as_ref()) else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    if request.guest_ids.is_empty() {
        return Err(BackendErrorTemplate::BookingNoGuestIds.into());
    }

    if Room::find(&request.room_id)
        .await?
        .hotel_id
        .ne(&path.hotel_id)
    {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    // TODO: Check if all guest ids are related to the hotel id in the path
    Booking::check_check_in_at_and_check_out_at(&request.check_in_at, &request.check_out_at)?;
    Booking::check_availability(
        &path.hotel_id,
        &request.room_id,
        &request.check_in_at,
        &request.check_out_at,
    )
    .await?;

    let booking_with_guest_ids = Booking::create(
        path.hotel_id,
        request.room_id,
        request.check_in_at,
        request.check_out_at,
        request.guest_ids,
    )
    .await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&booking_with_guest_ids)?))
}

#[derive(Deserialize)]
struct GetBookingsPath {
    hotel_id: i64,
}

#[get("/hotels/{hotel_id}/bookings")]
async fn get_all(
    session: ReqData<Session>,
    path: Path<GetBookingsPath>,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let bookings = Booking::find_all_by_hotel_id(&path.hotel_id).await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&bookings)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(create);
    cfg.service(get_all);
}
