use std::env;
use std::sync::OnceLock;

use sqlx::postgres::PgPoolOptions;
use sqlx::{Pool, Postgres};

static POOL: OnceLock<Pool<Postgres>> = OnceLock::new();

pub fn get_connection() -> &'static Pool<Postgres> {
    POOL.get().expect("Database pool not initialized")
}

pub async fn init() {
    info!("Initialize the database pool");

    if POOL.get().is_some() {
        warn!("Database already initialized");
        return;
    }

    let database_url = env::var("DATABASE_URL").expect("DATABASE_URL not set");
    let database_max_connections = env::var("DATABASE_MAX_CONNECTIONS")
        .unwrap_or("50".into())
        .parse::<u32>()
        .expect("DATABASE_MAX_CONNECTIONS must be `u32`");

    let pool = PgPoolOptions::new()
        .max_connections(database_max_connections)
        .connect(&database_url)
        .await
        .expect("Failed to connect to the database");

    POOL.set(pool).expect("Failed to initialize the database");

    let _ = get_connection();
}
