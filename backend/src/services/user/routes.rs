use actix_web::web::{ReqData, ServiceConfig};
use actix_web::{get, HttpResponse};

use crate::constants::DEFAULT_CONTENT_TYPE;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::session::Session;
use crate::services::user::User;

#[get("/users/@me")]
async fn get_me(session: ReqData<Session>) -> Result<HttpResponse, BackendError> {
    let Some(user_id) = session.user_id else {
        return Err(BackendErrorTemplate::Forbidden.into());
    };

    let user = User::find(&user_id).await?;

    Ok(HttpResponse::Ok()
        .content_type(DEFAULT_CONTENT_TYPE)
        .body(rmp_serde::to_vec(&user)?))
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(get_me);
}
