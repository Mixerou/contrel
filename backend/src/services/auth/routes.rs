use actix_web::web::{Bytes, ReqData, ServiceConfig};
use actix_web::{post, HttpResponse};

use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::auth::model::{LoginRequest, RegistrationRequest};
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

    Auth::register(credentials.to_owned()).await?;
    Auth::login(LoginRequest::from(credentials), session.id).await?;

    Ok(HttpResponse::Created().finish())
}

#[post("/auth/login")]
async fn login(session: ReqData<Session>, payload: Bytes) -> Result<HttpResponse, BackendError> {
    if session.user_id.is_some() {
        return Err(BackendErrorTemplate::LoggedInRestriction.into());
    }

    let Ok(credentials) = rmp_serde::from_slice::<LoginRequest>(payload.iter().as_slice()) else {
        return Err(BackendErrorTemplate::BadRequest.into());
    };

    Auth::login(credentials, session.id).await?;

    Ok(HttpResponse::NoContent().finish())
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(register);
    cfg.service(login);
}
