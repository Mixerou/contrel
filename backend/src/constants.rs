use actix_web::http::header::ContentType;

// HTTP codes
pub const HTTP_CODE_BAD_REQUEST: u16 = 400;
pub const HTTP_CODE_UNAUTHORIZED: u16 = 401;
pub const HTTP_CODE_FORBIDDEN: u16 = 403;
pub const HTTP_CODE_NOT_FOUND: u16 = 404;
pub const HTTP_CODE_CONFLICT: u16 = 409;
pub const HTTP_CODE_TOO_MANY_REQUESTS: u16 = 429;
pub const HTTP_CODE_INTERNAL_SERVER_ERROR: u16 = 500;

// Content Types
pub const DEFAULT_CONTENT_TYPE: ContentType = ContentType(mime::APPLICATION_MSGPACK);
