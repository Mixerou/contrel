use std::pin::pin;
use std::time::Instant;

use actix_web::web::Bytes;
use actix_ws::AggregatedMessage;
use futures_util::future::{select, Either};
use futures_util::StreamExt;
use tokio::sync::{mpsc, oneshot};
use tokio::time::interval;

use crate::constants::{
    HTTP_CODE_NOT_FOUND, WEB_SOCKET_CLIENT_TIMEOUT, WEB_SOCKET_HEARTBEAT_INTERVAL,
};
use crate::error::WebSocketCloseError;
use crate::services::session::Session;
use crate::web_socket::message::Opcode;
use crate::web_socket::{
    WebSocketCommand, WebSocketConnection, WebSocketMessage, WebSocketMessagePayload,
};

#[derive(Clone)]
pub struct WebSocketHandler {
    transmitter: mpsc::UnboundedSender<WebSocketCommand>,
}

impl WebSocketHandler {
    pub fn new(transmitter: mpsc::UnboundedSender<WebSocketCommand>) -> Self {
        Self { transmitter }
    }

    pub async fn handle_session(
        self,
        mut session: actix_ws::Session,
        message_stream: actix_ws::MessageStream,
    ) {
        let (connection_transmitter, mut connection_receiver) = mpsc::unbounded_channel();
        let mut connection = match self.connect(connection_transmitter).await {
            Ok(connection) => connection,
            Err(close_error) => {
                let _ = session.close(Some(close_error.into())).await;
                return;
            }
        };

        let message_stream = message_stream
            .max_frame_size(128 * 1024)
            .aggregate_continuations()
            .max_continuation_size(2 * 1024 * 1024);
        let mut message_stream = pin!(message_stream);

        let mut interval = interval(WEB_SOCKET_HEARTBEAT_INTERVAL);

        let close_reason = loop {
            let message_receiver = pin!(connection_receiver.recv());
            let messages = pin!(select(message_stream.next(), message_receiver));
            let tick = pin!(interval.tick());

            match select(messages, tick).await {
                Either::Left((Either::Left((Some(Ok(message)), _)), _)) => {
                    match message {
                        AggregatedMessage::Text(_) => {
                            break Some(WebSocketCloseError::Unsupported.into());
                        }
                        AggregatedMessage::Binary(bin) => {
                            if let Err(error) = self
                                .handle_message(&mut session, &mut connection, bin)
                                .await
                            {
                                break Some(error.into());
                            }
                        }
                        AggregatedMessage::Close(reason) => break reason,
                        _ => {} // I made a custom ping/pong
                    }
                }
                Either::Left((Either::Left((Some(Err(_)), _)), _)) => {
                    break Some(WebSocketCloseError::Protocol.into());
                }
                Either::Left((Either::Left((None, _)), _)) => {
                    break Some(WebSocketCloseError::Invalid.into());
                }
                Either::Left((Either::Right((Some(message), _)), _)) => {
                    if let Err(error) = Self::send_message(&mut session, message).await {
                        break Some(error.into());
                    }
                }
                Either::Left((Either::Right((None, _)), _)) => {
                    unreachable!("Lost connection with WebSocket")
                }
                Either::Right((_, _)) => {
                    if Instant::now().duration_since(connection.last_heartbeat_at)
                        > WEB_SOCKET_CLIENT_TIMEOUT
                    {
                        break Some(WebSocketCloseError::Normal.into());
                    }
                }
            };
        };

        if let Err(error) = self.disconnect(connection) {
            let _ = session.close(Some(error.into())).await;
            return;
        }

        let _ = session.close(close_reason).await;
    }

    async fn send_message(
        session: &mut actix_ws::Session,
        message: WebSocketMessage,
    ) -> Result<(), WebSocketCloseError> {
        let Ok(message) = rmp_serde::to_vec(&message) else {
            return Err(WebSocketCloseError::Error);
        };

        if session.binary(message).await.is_err() {
            return Err(WebSocketCloseError::Protocol);
        }

        Ok(())
    }

    async fn handle_message(
        &self,
        session: &mut actix_ws::Session,
        connection: &mut WebSocketConnection,
        message: Bytes,
    ) -> Result<(), WebSocketCloseError> {
        let Ok(message) = rmp_serde::from_slice::<WebSocketMessage>(&message) else {
            return Err(WebSocketCloseError::Invalid);
        };

        match connection.session_token {
            Some(_) if message.opcode.eq(&Opcode::Authorize) => {
                return Err(WebSocketCloseError::AlreadyAuthenticated);
            }
            Some(_) => {}
            None if message.opcode.ne(&Opcode::Authorize) => {
                return Err(WebSocketCloseError::NotAuthenticated);
            }
            None => {
                let WebSocketMessagePayload::Authorize { token } = message.payload else {
                    return Err(WebSocketCloseError::Invalid);
                };

                match Session::find_by_token(&token).await {
                    Ok(session) if session.user_id.is_some() => {
                        connection.session_token = Some(session.token);
                    }
                    Ok(_) => {
                        return Err(WebSocketCloseError::AuthenticationFailed);
                    }
                    Err(error) if error.http_code.eq(&HTTP_CODE_NOT_FOUND) => {
                        return Err(WebSocketCloseError::AuthenticationFailed);
                    }
                    Err(_) => {
                        return Err(WebSocketCloseError::Error);
                    }
                }

                let response = WebSocketMessage {
                    id: message.id,
                    ..Default::default()
                };
                Self::send_message(session, response).await?;

                return Ok(());
            }
        }

        match message.opcode {
            Opcode::HeartBeat => {
                connection.last_heartbeat_at = Instant::now();

                let response = WebSocketMessage {
                    id: message.id,
                    ..Default::default()
                };
                Self::send_message(session, response).await?;
            }
            Opcode::Request => match message.payload {
                WebSocketMessagePayload::RequestPostTestMessage {
                    message: test_message,
                } => {
                    let response = WebSocketMessage {
                        id: message.id,
                        payload: WebSocketMessagePayload::ResponseTestMessage {
                            message: test_message,
                        },
                        ..Default::default()
                    };
                    Self::send_message(session, response).await?;
                }
                _ => {}
            },
            Opcode::Response => {}
            Opcode::Error => return Err(WebSocketCloseError::Opcode),
            Opcode::Authorize => {}
        }

        Ok(())
    }

    async fn connect(
        &self,
        connection_transmitter: mpsc::UnboundedSender<WebSocketMessage>,
    ) -> Result<WebSocketConnection, WebSocketCloseError> {
        let (response_transmitter, response_receiver) = oneshot::channel();

        if self
            .transmitter
            .send(WebSocketCommand::Connect {
                connection_transmitter,
                result_transmitter: response_transmitter,
            })
            .is_err()
        {
            return Err(WebSocketCloseError::Error);
        }

        match response_receiver.await {
            Ok(connection) => Ok(connection),
            Err(_) => Err(WebSocketCloseError::Protocol),
        }
    }

    async fn dispatch(
        &self,
        connection: WebSocketConnection,
        message: impl Into<WebSocketMessage>,
    ) -> Result<(), WebSocketCloseError> {
        let (result_transmitter, result_receiver) = oneshot::channel();

        if self
            .transmitter
            .send(WebSocketCommand::Dispatch {
                connections: vec![connection],
                message: message.into(),
                result_transmitter,
            })
            .is_err()
        {
            return Err(WebSocketCloseError::Error);
        }

        if result_receiver.await.is_err() {
            return Err(WebSocketCloseError::Protocol);
        }

        Ok(())
    }

    fn disconnect(&self, connection: WebSocketConnection) -> Result<(), WebSocketCloseError> {
        if self
            .transmitter
            .send(WebSocketCommand::Disconnect { connection })
            .is_err()
        {
            return Err(WebSocketCloseError::Error);
        }

        Ok(())
    }
}
