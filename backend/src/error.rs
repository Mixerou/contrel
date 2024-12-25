use std::fmt;

use actix_web::http::header::ToStrError as ActixToStrError;
use actix_web::http::StatusCode;
use actix_web::{Error as ActixWebError, HttpResponse, ResponseError};
use actix_ws::{CloseCode, CloseReason};
use argon2::password_hash::Error as Argon2PasswordHashError;
use argon2::Error as Argon2Error;
use rmp_serde::encode::Error as RmpEncodeError;
use serde::{Deserialize, Serialize};
use sqlx::error::ErrorKind as SqlxErrorKind;
use sqlx::Error as SqlxError;

use crate::constants::{
    DEFAULT_CONTENT_TYPE, HTTP_CODE_BAD_REQUEST, HTTP_CODE_CONFLICT, HTTP_CODE_FORBIDDEN,
    HTTP_CODE_INTERNAL_SERVER_ERROR, HTTP_CODE_NOT_FOUND, HTTP_CODE_TOO_MANY_REQUESTS,
    HTTP_CODE_UNAUTHORIZED,
};

#[derive(Debug)]
#[allow(dead_code)]
pub enum BackendErrorKind {
    // Dependencies
    ActixToStr(ActixToStrError),
    ActixWeb(ActixWebError),
    Argon2(Argon2Error),
    Argon2PasswordHash(Argon2PasswordHashError),
    RmpEncode(RmpEncodeError),
    Sqlx(SqlxError),

    // Other
    Other(Option<String>),
}

impl Default for BackendErrorKind {
    fn default() -> Self {
        Self::Other(None)
    }
}

#[derive(Debug, Deserialize)]
pub struct BackendError {
    #[serde(skip)]
    pub http_code: u16,
    pub code: u32,
    pub message: String,
    #[serde(skip)]
    pub kind: BackendErrorKind,
}

#[allow(dead_code)]
impl BackendError {
    pub fn new(
        http_code: u16,
        code: Option<u32>,
        message: String,
        error: Option<BackendErrorKind>,
    ) -> Self {
        Self {
            http_code,
            code: code.unwrap_or(http_code as u32),
            message,
            kind: error.unwrap_or(BackendErrorKind::Other(None)),
        }
    }

    pub fn new_internal(message: String, error: BackendErrorKind) -> Self {
        Self {
            http_code: HTTP_CODE_INTERNAL_SERVER_ERROR,
            code: HTTP_CODE_INTERNAL_SERVER_ERROR.into(),
            message,
            kind: error,
        }
    }

    pub fn get_safe_message(&self) -> String {
        match self.http_code < 500 {
            true => self.message.to_owned(),
            false => {
                error!("{} >> {:?}", self.message, self.kind);
                "Internal server error".into()
            }
        }
    }

    pub fn set_message(mut self, message: String) -> Self {
        self.message = message;
        self
    }

    pub fn set_kind(mut self, kind: BackendErrorKind) -> Self {
        self.kind = kind;
        self
    }
}

impl fmt::Display for BackendError {
    fn fmt(&self, formatter: &mut fmt::Formatter) -> fmt::Result {
        formatter.write_str(self.message.as_str())
    }
}

// From dependency errors
impl From<ActixToStrError> for BackendError {
    fn from(error: ActixToStrError) -> Self {
        Self::new_internal(
            format!("Actix `to_str` error: {error}"),
            BackendErrorKind::ActixToStr(error),
        )
    }
}

impl From<ActixWebError> for BackendError {
    fn from(error: ActixWebError) -> Self {
        Self::new_internal(
            format!("Actix Web error: {error}"),
            BackendErrorKind::ActixWeb(error),
        )
    }
}

impl From<Argon2Error> for BackendError {
    fn from(error: Argon2Error) -> Self {
        Self::new_internal(
            format!("Argon2 error: {error}"),
            BackendErrorKind::Argon2(error),
        )
    }
}

impl From<Argon2PasswordHashError> for BackendError {
    fn from(error: Argon2PasswordHashError) -> Self {
        Self::new_internal(
            format!("Argon2 password hash error: {error}"),
            BackendErrorKind::Argon2PasswordHash(error),
        )
    }
}

impl From<RmpEncodeError> for BackendError {
    fn from(error: RmpEncodeError) -> Self {
        Self::new_internal(
            format!("RMP encode error: {error}"),
            BackendErrorKind::RmpEncode(error),
        )
    }
}

impl From<SqlxError> for BackendError {
    fn from(error: SqlxError) -> Self {
        match error {
            SqlxError::RowNotFound => BackendErrorTemplate::NotFound
                .error()
                .set_kind(BackendErrorKind::Sqlx(error)),
            SqlxError::Database(ref db_error) => match db_error.kind() {
                SqlxErrorKind::UniqueViolation => BackendErrorTemplate::Conflict
                    .error()
                    .set_kind(BackendErrorKind::Sqlx(error)),
                _ => Self::new_internal(
                    format!("Sqlx database error: {db_error}"),
                    BackendErrorKind::Sqlx(error),
                ),
            },
            error => Self::new_internal(
                format!("Sqlx error: {error}"),
                BackendErrorKind::Sqlx(error),
            ),
        }
    }
}

impl ResponseError for BackendError {
    fn error_response(&self) -> HttpResponse {
        let status_code =
            StatusCode::from_u16(self.http_code).unwrap_or(StatusCode::INTERNAL_SERVER_ERROR);
        let message = self.get_safe_message();

        #[derive(Serialize)]
        struct ResponseBody {
            code: u32,
            message: String,
        }

        match rmp_serde::to_vec(&ResponseBody {
            code: self.code,
            message,
        }) {
            Ok(message) => HttpResponse::build(status_code)
                .content_type(DEFAULT_CONTENT_TYPE)
                .body(message),
            Err(error) => {
                error!("Error response serialization error: {error}");
                HttpResponse::InternalServerError().finish()
            }
        }
    }
}

macro_rules! backend_error_template {
    ($(($http_code:ident, $code:expr, $name:ident, $message:expr);)+) => {
        #[allow(dead_code)]
        pub enum BackendErrorTemplate {
            $( $name, )+
        }

        #[allow(dead_code)]
        impl BackendErrorTemplate {
            pub fn error(self) -> BackendError {
                self.into()
            }
        }

        impl From<BackendErrorTemplate> for BackendError {
            fn from(template: BackendErrorTemplate) -> Self {
                match template {
                $(
                    BackendErrorTemplate::$name => {
                        Self::new($http_code, $code, $message.into(), None)
                    }
                )+
                }
            }
        }
    }
}

backend_error_template! {
    // Default HTTP errors
    (HTTP_CODE_BAD_REQUEST, None, BadRequest, "Bad request");
    (HTTP_CODE_UNAUTHORIZED, None, Unauthorized, "Unauthorized");
    (HTTP_CODE_FORBIDDEN, None, Forbidden, "Forbidden");
    (HTTP_CODE_NOT_FOUND, None, NotFound, "Not found");
    (HTTP_CODE_CONFLICT, None, Conflict, "Conflict");
    (HTTP_CODE_TOO_MANY_REQUESTS, None, TooManyRequests, "Too many requests");
    (HTTP_CODE_INTERNAL_SERVER_ERROR, None, InternalServerError, "Internal server error");

    // Unknown entities
    // The first error (1000) is virtually, the same as the standard 404 HTTP error.
    // It can be used for the general / unknown 1xxx error.
    (HTTP_CODE_NOT_FOUND, Some(1000), UnknownEntity, "Entity not found");

    // Restrictions
    //
    // The first error (2000) is virtually, the same as the standard 403 HTTP error.
    // It can be used for the general / unknown 2xxx error.
    //
    // The second error (2001) is virtually, the same as the standard 429 HTTP error.
    // It can be used for any unknown actions that required to be rate limited.
    (HTTP_CODE_BAD_REQUEST, Some(2000), Restricted, "You are unable to perform this action");
    (HTTP_CODE_TOO_MANY_REQUESTS, Some(2001), RateLimit, "You are rate limited");

    // Minimum / Maximum number of ... reached
    // The first error (3000) can be used for the general / unknown 3xxx error.
    (HTTP_CODE_BAD_REQUEST, Some(3000), ReachedEdge, "Minimum or maximum reached");
    (HTTP_CODE_BAD_REQUEST, Some(3001), UserPasswordTooShort, "User password is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3002), UserPasswordTooLong, "User password is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3003), UserEmailTooShort, "User email is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3004), UserEmailTooLong, "User email is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3005), UserFirstNameTooShort, "First name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3006), UserFirstNameTooLong, "First name is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3007), UserLastNameTooShort, "Last name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3008), UserLastNameTooLong, "Last name is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3009), HotelNameTooShort, "Hotel name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3010), HotelNameTooLong, "Hotel name is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3011), HotelAlreadyExists, "You already have a hotel with that name");
    (HTTP_CODE_BAD_REQUEST, Some(3012), GuestFirstNameTooShort, "First name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3013), GuestFirstNameTooLong, "First name is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3014), GuestLastNameTooShort, "Last name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3015), GuestLastNameTooLong, "Last name is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3016), GuestPhoneNumberTooShort, "Guest phone number is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3017), GuestPhoneNumberTooLong, "Guest phone number is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3018), GuestEmailTooShort, "Guest email is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3019), GuestEmailTooLong, "Guest email is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3020), GuestDocumentNumberTooShort, "Guest document number is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3021), GuestDocumentNumberTooLong, "Guest document number is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3022), GuestNotesTooLong, "Guest notes are too long");
    (HTTP_CODE_BAD_REQUEST, Some(3023), RoomNumberTooShort, "Room number is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3024), RoomNumberTooLong, "Room number is too long");
    (HTTP_CODE_BAD_REQUEST, Some(3025), RoomGroupNameTooShort, "Room group name is too short");
    (HTTP_CODE_BAD_REQUEST, Some(3026), RoomGroupNameTooLong, "Room group name is too long");

    // Invalid body or something else
    // The first error (4000) is virtually, the same as the standard 400 HTTP error.
    // It can be used for the general / unknown 4xxx error.
    (HTTP_CODE_BAD_REQUEST, Some(4000), InvalidRequest, "Invalid request");
    (HTTP_CODE_BAD_REQUEST, Some(4001), BadEmail, "Bad email");
    (HTTP_CODE_FORBIDDEN, Some(4002), LoggedInRestriction, "This action cannot be performed while you are logged in");
    (HTTP_CODE_BAD_REQUEST, Some(4003), AlreadyRegistered, "Account with this email already registered");
    (HTTP_CODE_BAD_REQUEST, Some(4004), BadLoginCredentials, "Bad login credentials");
    (HTTP_CODE_BAD_REQUEST, Some(4005), FutureDateOfBirth, "The date of birth can't be in the future");
    (HTTP_CODE_BAD_REQUEST, Some(4006), GuestPastDocumentValidUntil, "The document valid date can't be in the past");
    (HTTP_CODE_BAD_REQUEST, Some(4007), BadPhoneNumber, "Bad phone number");
}

macro_rules! websocket_close_error {
    ( $( ($code:expr, $name:ident, $description:expr); )+ ) => {
        #[allow(dead_code)]
        #[repr(u16)]
        pub enum WebSocketCloseError {
            $( $name = $code, )+
        }

        impl WebSocketCloseError {
            pub fn get_close_reason(&self) -> CloseReason {
                match self {
                    $(
                        Self::$name => {
                            CloseReason {
                                code: CloseCode::Other($code),
                                description: Some($description.to_string()),
                            }
                        }
                    )+
                }
            }
        }

        impl From<WebSocketCloseError> for CloseReason {
            fn from(error: WebSocketCloseError) -> Self {
                error.get_close_reason()
            }
        }
    }
}

websocket_close_error! {
    (1000, Normal, "Connection purpose fulfilled");
    (1001, Away, "Endpoint gone");
    (1002, Protocol, "Protocol error detected");
    (1003, Unsupported, "Unsupported data type received");
    (1006, Abnormal, "Connection dropped unexpectedly");
    (1007, Invalid, "Inconsistent message data");
    (1008, Policy, "Policy violation detected");
    (1009, Size, "Message too large");
    (1010, Extension, "Missing expected extension");
    (1011, Error, "Unexpected server condition");
    (1012, Restart, "Server rebooting");
    (1013, Again, "Server overloaded, retry");
    (1015, Tls, "TLS issue");

    (4000, Unknown, "Unknown error");
    (4001, Opcode, "Opcode not allowed");
    (4002, NotAuthenticated, "Not authenticated");
    (4003, AuthenticationFailed, "Authentication failed");
    (4004, AlreadyAuthenticated, "Already authenticated");
}
