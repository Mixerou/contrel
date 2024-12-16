use actix_web::web::{Bytes, ReqData, ServiceConfig};
use actix_web::{post, HttpResponse};

use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::auth::model::RegistrationRequest;
use crate::services::auth::Auth;
use crate::services::session::Session;

#[post("/auth/register")]
async fn register(session: ReqData<Session>, payload: Bytes) -> Result<HttpResponse, BackendError> {
    if session.user_id.is_some() {
        return Err(BackendErrorTemplate::LoggedInRestriction.into());
    }

    let Ok(credentials) = rmp_serde::from_slice::<RegistrationRequest>(payload.iter().as_slice())
    else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    Auth::register(credentials).await?;

    Ok(HttpResponse::Created().finish())
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(register);
}
