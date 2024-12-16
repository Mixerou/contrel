use serde::Deserialize;

use crate::constants::HTTP_CODE_NOT_FOUND;
use crate::error::{BackendError, BackendErrorTemplate};
use crate::services::user::User;
use crate::utils::argon::hash_password;

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
}

#[derive(Clone, Deserialize)]
pub(super) struct RegistrationRequest {
    pub email: String,
    pub password: String,
    pub first_name: String,
    pub last_name: String,
}
