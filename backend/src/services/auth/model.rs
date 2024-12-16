use serde::Deserialize;

use crate::constants::HTTP_CODE_NOT_FOUND;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::session::Session;
use crate::services::user::User;
use crate::utils::argon::{hash_password, verify_password};

pub(super) struct Auth;

impl Auth {
    pub async fn register(request: RegistrationRequest) -> Result<User, BackendError> {
        User::check_email(&request.email)?;
        User::check_password_length(&request.password)?;
        User::check_first_name(&request.first_name)?;
        User::check_last_name(&request.last_name)?;

        match User::find_by_email(&request.email).await {
            Ok(_) => Err(BackendErrorTemplate::AlreadyRegistered.into()),
            Err(error) if error.http_code == HTTP_CODE_NOT_FOUND => {
                let password_hash = hash_password(request.password)?;

                Ok(User::create(
                    request.email,
                    password_hash,
                    request.first_name,
                    request.last_name,
                )
                .await?)
            }
            Err(error) => Err(error),
        }
    }

    pub async fn login(request: LoginRequest, session_id: i64) -> Result<(), BackendError> {
        User::check_email(&request.email)?;
        User::check_password_length(&request.password)?;

        let user = match User::find_by_email(&request.email).await {
            Ok(user) => user,
            Err(error) if error.http_code == HTTP_CODE_NOT_FOUND => {
                return Err(BackendErrorTemplate::BadLoginCredentials.into())
            }
            Err(error) => return Err(error),
        };

        if verify_password(request.password, user.password_hash).is_err() {
            return Err(BackendErrorTemplate::BadLoginCredentials.into());
        }

        Session::update_user_id(&session_id, Some(user.id)).await?;

        Ok(())
    }
}

#[derive(Clone, Deserialize)]
pub(super) struct RegistrationRequest {
    pub email: String,
    pub password: String,
    pub first_name: String,
    pub last_name: String,
}

#[derive(Deserialize)]
pub(super) struct LoginRequest {
    pub email: String,
    pub password: String,
}

impl From<RegistrationRequest> for LoginRequest {
    fn from(request: RegistrationRequest) -> Self {
        Self {
            email: request.email,
            password: request.password,
        }
    }
}
