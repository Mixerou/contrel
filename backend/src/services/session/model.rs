use nanoid::nanoid;
use sqlx::{query_as, FromRow};
use time::PrimitiveDateTime;

use crate::error::BackendError;
use crate::{db, snowflake_generator};

#[derive(Clone, FromRow)]
#[allow(dead_code)]
pub struct Session {
    pub id: i64,
    pub token: String,
    pub created_at: PrimitiveDateTime,
    pub user_id: Option<i64>,
}

impl Session {
    pub async fn create() -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let session = query_as!(
            Self,
            r#"INSERT INTO sessions (id, token) VALUES ($1, $2) RETURNING *"#,
            snowflake_generator::generate(),
            format!("{}{}", nanoid!(45), snowflake_generator::generate()),
        )
        .fetch_one(connection)
        .await?;

        Ok(session)
    }

    pub async fn find_by_token(token: &str) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let session = query_as!(
            Self,
            r#"
                SELECT *
                FROM sessions
                WHERE token = $1
            "#,
            token,
        )
        .fetch_one(connection)
        .await?;

        Ok(session)
    }

    pub async fn update_user_id(id: &i64, user_id: Option<i64>) -> Result<Self, BackendError> {
        let connection = db::get_connection();

        let session = query_as!(
            Self,
            r#"UPDATE sessions SET user_id = $1 WHERE id = $2 RETURNING *"#,
            user_id,
            id,
        )
        .fetch_one(connection)
        .await?;

        Ok(session)
    }
}
