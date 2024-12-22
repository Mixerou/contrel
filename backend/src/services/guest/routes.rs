use actix_web::web::{Bytes, Path, ReqData, ServiceConfig};
use actix_web::{get, post, HttpResponse};
use serde::Deserialize;
use time::PrimitiveDateTime;

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::guest::{DocumentType, Guest};
use crate::services::hotel::Hotel;
use crate::services::session::Session;
use crate::types::{Country, Gender};
use crate::utils::time::deserialize_from_unix_timestamp;

#[derive(Deserialize)]
struct GuestCreationPath {
    hotel_id: i64,
}

#[derive(Deserialize)]
struct GuestCreationRequest {
    first_name: String,
    last_name: String,
    #[serde(deserialize_with = "deserialize_from_unix_timestamp")]
    date_of_birth: PrimitiveDateTime,
    gender: Gender,
    phone_number: String,
    email: String,
    document_type: DocumentType,
    document_number: String,
    document_country: Country,
    #[serde(deserialize_with = "deserialize_from_unix_timestamp")]
    document_valid_until: PrimitiveDateTime,
    notes: String,
}

#[post("/hotels/{hotel_id}/guests")]
async fn create(
    session: ReqData<Session>,
    path: Path<GuestCreationPath>,
    payload: Bytes,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let Ok(guest) = rmp_serde::from_slice::<GuestCreationRequest>(payload.as_ref()) else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    Guest::check_first_name(&guest.first_name)?;
    Guest::check_last_name(&guest.last_name)?;
    Guest::check_date_of_birth(&guest.date_of_birth)?;
    Guest::check_phone_number(&guest.phone_number)?;
    Guest::check_email(&guest.email)?;
    Guest::check_document_number(&guest.document_number)?;
    Guest::check_document_valid_until(&guest.document_valid_until)?;
    Guest::check_notes(&guest.notes)?;

    let guest = Guest::create(
        guest.first_name,
        guest.last_name,
        guest.date_of_birth,
        guest.gender,
        guest.phone_number,
        guest.email,
        guest.document_type,
        guest.document_number,
        guest.document_country,
        guest.document_valid_until,
        guest.notes,
        path.hotel_id,
    )
    .await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&guest)?))
}

#[derive(Deserialize)]
struct GetGuestPath {
    hotel_id: i64,
}

#[get("/hotels/{hotel_id}/guests")]
async fn get_all(
    session: ReqData<Session>,
    path: Path<GetGuestPath>,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let guests = Guest::find_all_by_hotel_id(&path.hotel_id).await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&guests)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(create);
    cfg.service(get_all);
}
