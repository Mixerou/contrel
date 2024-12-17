use std::collections::HashMap;
use std::io;
use std::time::Instant;

use tokio::sync::mpsc;

use crate::snowflake_generator;
use crate::web_socket::handler::WebSocketHandler;
use crate::web_socket::{WebSocketCommand, WebSocketConnection, WebSocketMessage};

pub struct WebSocket {
    sessions: HashMap<i64, mpsc::UnboundedSender<WebSocketMessage>>,
    command_receiver: mpsc::UnboundedReceiver<WebSocketCommand>,
}

impl WebSocket {
    pub fn new() -> (Self, WebSocketHandler) {
        let (command_transmitter, command_receiver) = mpsc::unbounded_channel();

        (
            Self {
                sessions: HashMap::new(),
                command_receiver,
            },
            WebSocketHandler::new(command_transmitter),
        )
    }

    pub async fn run(mut self) -> io::Result<()> {
        while let Some(command) = self.command_receiver.recv().await {
            match command {
                WebSocketCommand::Connect {
                    connection_transmitter,
                    result_transmitter,
                } => {
                    let connection_id = self.connect(connection_transmitter).await;
                    let _ = result_transmitter.send(connection_id);
                }
                WebSocketCommand::Disconnect {
                    connection: connection_id,
                } => {
                    self.disconnect(connection_id).await;
                }
                WebSocketCommand::Dispatch {
                    connections,
                    message,
                    result_transmitter,
                } => {
                    self.dispatch(connections, message).await;
                    let _ = result_transmitter.send(());
                }
            }
        }

        Ok(())
    }

    async fn connect(
        &mut self,
        tx: mpsc::UnboundedSender<WebSocketMessage>,
    ) -> WebSocketConnection {
        let connection_id = snowflake_generator::generate();
        self.sessions.insert(connection_id, tx);

        WebSocketConnection {
            id: connection_id,
            session_token: None,
            last_heartbeat_at: Instant::now(),
        }
    }

    async fn dispatch(
        &self,
        connections: Vec<WebSocketConnection>,
        message: impl Into<WebSocketMessage>,
    ) {
        // Nothing to dispatch
    }

    async fn disconnect(&mut self, connection: WebSocketConnection) {
        self.sessions.remove(&connection.id);
    }
}
