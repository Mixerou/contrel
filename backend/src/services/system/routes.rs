use actix_web::web::ServiceConfig;
use actix_web::{get, HttpResponse};

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::BackendError;

#[get("/ping")]
pub async fn ping() -> Result<HttpResponse, BackendError> {
    Ok(HttpResponse::Ok()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec("pong")?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(ping);
}
