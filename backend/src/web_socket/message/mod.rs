mod payload;

use serde::{Deserialize, Serialize};
use serde_repr::{Deserialize_repr, Serialize_repr};

pub use crate::web_socket::message::payload::WebSocketMessagePayload;

#[derive(Debug, Default, Deserialize_repr, Serialize_repr, Eq, PartialEq)]
#[repr(u8)]
pub enum Opcode {
    HeartBeat = 0,
    Request = 1,
    #[default]
    Response = 2,
    Error = 3,
    Authorize = 4,
}

#[derive(Debug, Default, Deserialize, Serialize)]
pub struct WebSocketMessage {
    pub id: i64,
    pub opcode: Opcode,
    #[serde(
        default = "WebSocketMessagePayload::default",
        skip_serializing_if = "WebSocketMessagePayload::is_none"
    )]
    pub payload: WebSocketMessagePayload,
}
