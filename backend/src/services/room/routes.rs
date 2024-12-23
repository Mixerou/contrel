use actix_web::web::{Bytes, Path, ReqData, ServiceConfig};
use actix_web::{get, post, HttpResponse};
use serde::Deserialize;

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::hotel::Hotel;
use crate::services::room::Room;
use crate::services::session::Session;

#[derive(Deserialize)]
struct RoomCreationPath {
    hotel_id: i64,
}

#[derive(Deserialize)]
struct RoomCreationRequest {
    number: String,
    group_name: String,
}

#[post("/hotels/{hotel_id}/rooms")]
async fn create(
    session: ReqData<Session>,
    path: Path<RoomCreationPath>,
    payload: Bytes,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let Ok(room) = rmp_serde::from_slice::<RoomCreationRequest>(payload.as_ref()) else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    Room::check_number(&room.number)?;
    Room::check_group_name(&room.group_name)?;

    let room = Room::create(room.number, room.group_name, path.hotel_id).await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&room)?))
}

#[derive(Deserialize)]
struct GetRoomsPath {
    hotel_id: i64,
}

#[get("/hotels/{hotel_id}/rooms")]
async fn get_all(
    session: ReqData<Session>,
    path: Path<GetRoomsPath>,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let rooms = Room::find_all_by_hotel_id(&path.hotel_id).await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&rooms)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(create);
    cfg.service(get_all);
}
