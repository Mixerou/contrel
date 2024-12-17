use argon2::password_hash::rand_core::OsRng;
use argon2::password_hash::SaltString;
use argon2::{Algorithm, Argon2, Params, PasswordHash, PasswordHasher, PasswordVerifier, Version};

use crate::error::BackendError;

pub fn hash_password<T: ToString>(password: T) -> Result<String, BackendError> {
    let salt = SaltString::generate(&mut OsRng);
    let params = Params::new(65536, 8, 4, None);
    let argon2 = Argon2::new(Algorithm::Argon2id, Version::V0x13, params?);

    let password_hash = argon2.hash_password(password.to_string().as_bytes(), &salt)?;

    Ok(password_hash.to_string())
}

pub fn verify_password<T: ToString, S: ToString>(
    password: T,
    password_hash: S,
) -> Result<(), BackendError> {
    let password_hash = password_hash.to_string();
    let password_hash = PasswordHash::new(&password_hash)?;

    Ok(Argon2::default().verify_password(password.to_string().as_bytes(), &password_hash)?)
}
