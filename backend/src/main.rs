#[macro_use]
extern crate log;

use std::env;

use actix_web::middleware::{NormalizePath, TrailingSlash};
use actix_web::{web, App, HttpServer};
use dotenv::dotenv;

mod constants;
mod db;
mod error;
mod services;
mod snowflake_generator;

#[actix_web::main]
async fn main() -> std::io::Result<()> {
    dotenv().ok();
    env_logger::init();

    db::init().await;
    snowflake_generator::init();

    let ip = env::var("BACKEND_IP").unwrap_or_else(|_| "127.0.0.1".to_string());
    let port = env::var("BACKEND_PORT").unwrap_or_else(|_| "5000".to_string());
    let api_path = env::var("BACKEND_API_PATH").unwrap_or_else(|_| "".to_string());

    info!("Starting backend module on {} with port {}", ip, port);

    HttpServer::new(move || {
        App::new().service(
            web::scope(api_path.as_str())
                .wrap(NormalizePath::new(TrailingSlash::Trim))
                .wrap(services::session::middleware::CheckSession)
                .configure(services::system::init_routes),
        )
    })
    .bind(format!("{}:{}", ip, port))?
    .run()
    .await
}
