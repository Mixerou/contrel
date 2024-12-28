use actix_web::web::{Path, ReqData, ServiceConfig};
use actix_web::{get, HttpResponse};
use serde::Deserialize;

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::hotel::Hotel;
use crate::services::session::Session;
use crate::services::stats::StatsLongTimeGuest;

#[derive(Deserialize)]
struct GetStatsLongTimeGuestsPath {
    hotel_id: i64,
}

#[get("/hotels/{hotel_id}/stats/long-time-guests")]
async fn get_long_time_guests(
    session: ReqData<Session>,
    path: Path<GetStatsLongTimeGuestsPath>,
) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    if Hotel::find(&path.hotel_id).await?.owner_id.ne(&user_id) {
        return Err(BackendErrorTemplate::NotFound.into());
    }

    let guests = StatsLongTimeGuest::find_all_by_hotel_id(&path.hotel_id).await?;

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&guests)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(get_long_time_guests);
}
