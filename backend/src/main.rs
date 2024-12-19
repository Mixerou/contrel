#[macro_use]
extern crate log;

use std::env;

use actix_web::middleware::{NormalizePath, TrailingSlash};
use actix_web::rt::spawn;
use actix_web::web::Data;
use actix_web::{web, App, HttpServer};
use dotenv::dotenv;
use tokio::try_join;

use crate::web_socket::WebSocket;

mod constants;
mod db;
mod error;
mod services;
mod snowflake_generator;
mod types;
mod utils;
mod web_socket;

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    dotenv().ok();
    env_logger::init();

    db::init().await;
    snowflake_generator::init();

    let ip = env::var("BACKEND_IP").unwrap_or_else(|_| "127.0.0.1".to_string());
    let port = env::var("BACKEND_PORT").unwrap_or_else(|_| "5000".to_string());
    let api_path = env::var("BACKEND_API_PATH").unwrap_or_else(|_| "".to_string());
    let web_socket_path =
        env::var("BACKEND_WEB_SOCKET_PATH").unwrap_or_else(|_| "/gateway".to_string());

    info!("Starting backend module on {} with port {}", ip, port);

    let (web_socket, web_socket_handler) = WebSocket::new();
    let web_socket = spawn(web_socket.run());

    HttpServer::new(move || {
        App::new()
            .service(
                web::scope(api_path.as_str())
                    .configure(services::auth::init_routes)
                    .configure(services::guest::init_routes)
                    .configure(services::hotel::init_routes)
                    .configure(services::system::init_routes)
                    .configure(services::user::init_routes)
                    .wrap(services::session::middleware::CheckSession),
            )
            .service(
                web::scope(web_socket_path.as_str())
                    .app_data(Data::new(web_socket_handler.clone()))
                    .configure(web_socket::init_routes)
                    .wrap(NormalizePath::new(TrailingSlash::Trim)),
            )
            .wrap(NormalizePath::new(TrailingSlash::Trim))
    })
    .bind(format!("{}:{}", ip, port))?
    .run()
    .await
    .expect("Failed HTTP server operation");

    try_join!(async move { web_socket.await.expect("Failed to join to the WebSocket") })?;

    Ok(())
}
