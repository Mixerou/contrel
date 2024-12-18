use actix_web::web::{Bytes, ReqData, ServiceConfig};
use actix_web::{post, HttpResponse};
use serde::Deserialize;

use crate::constants::{DEFAULT_CONTENT_TYPE, HTTP_CODE_CONFLICT};
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::hotel::Hotel;
use crate::services::session::Session;

#[derive(Deserialize)]
struct HotelCreationRequest {
    name: String,
}

#[post("/hotels")]
async fn create(session: ReqData<Session>, payload: Bytes) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    let Ok(hotel) = rmp_serde::from_slice::<HotelCreationRequest>(payload.as_ref()) else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    Hotel::check_name(&hotel.name)?;
    let hotel = match Hotel::create(hotel.name, user_id).await {
        Ok(hotel) => hotel,
        Err(error) if error.http_code.eq(&HTTP_CODE_CONFLICT) => {
            return Err(BackendErrorTemplate::HotelAlreadyExists.into());
        }
        Err(error) => return Err(error),
    };

    Ok(HttpResponse::Created()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&hotel)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(create);
}
