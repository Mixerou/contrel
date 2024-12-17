use std::time::Instant;

use tokio::sync::mpsc::UnboundedSender;
use tokio::sync::oneshot;

mod handler;
mod message;
mod routes;
mod socket;

pub use message::{WebSocketMessage, WebSocketMessagePayload};
pub use routes::init_routes;
pub use socket::WebSocket;

pub struct WebSocketConnection {
    id: i64,
    session_token: Option<String>,
    last_heartbeat_at: Instant,
}

pub enum WebSocketCommand {
    Connect {
        connection_transmitter: UnboundedSender<WebSocketMessage>,
        result_transmitter: oneshot::Sender<WebSocketConnection>,
    },
    Disconnect {
        connection: WebSocketConnection,
    },
    Dispatch {
        connections: Vec<WebSocketConnection>,
        message: WebSocketMessage,
        result_transmitter: oneshot::Sender<()>,
    },
}
