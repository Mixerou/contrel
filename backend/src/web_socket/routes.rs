use actix_web::web::{Data, Payload, ServiceConfig};
use actix_web::{get, HttpRequest, HttpResponse};
use tokio::task::spawn_local;

use crate::error::BackendError;
use crate::web_socket::handler::WebSocketHandler;

#[get("")]
async fn connect(
    request: HttpRequest,
    stream: Payload,
    web_socket_handler: Data<WebSocketHandler>,
) -> Result<HttpResponse, BackendError> {
    let (response, session, message_stream) = actix_ws::handle(&request, stream)?;

    spawn_local(WebSocketHandler::handle_session(
        (**web_socket_handler).clone(),
        session,
        message_stream,
    ));

    Ok(response)
}

pub fn init_routes(cfg: &mut ServiceConfig) {
    cfg.service(connect);
}
