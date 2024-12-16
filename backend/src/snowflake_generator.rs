use std::sync::{Mutex, OnceLock};
use std::time::{Duration, UNIX_EPOCH};

use snowflake::SnowflakeIdGenerator;

static SNOWFLAKE_ID_GENERATOR: OnceLock<Mutex<SnowflakeIdGenerator>> = OnceLock::new();

pub fn generate() -> i64 {
    let mut generator = SNOWFLAKE_ID_GENERATOR
        .get()
        .expect("Snowflake ID generator is not initialized")
        .lock()
        .expect("Mutex with Snowflake ID generator causes an error");

    generator.real_time_generate()
}

pub fn init() {
    info!("Initialize Snowflake Generator");

    let snowflake_id_generator =
        SnowflakeIdGenerator::with_epoch(0, 0, UNIX_EPOCH + Duration::from_millis(1733011200000));
    SNOWFLAKE_ID_GENERATOR
        .set(Mutex::new(snowflake_id_generator))
        .expect("Failed to initialize Snowflake ID generator");

    let _ = generate();
}
