CREATE TABLE "users"
(
    id            BIGINT PRIMARY KEY,
    email         VARCHAR(64) NOT NULL UNIQUE,
    password_hash TEXT        NOT NULL,
    first_name    VARCHAR(32) NOT NULL UNIQUE,
    last_name     VARCHAR(32) NOT NULL UNIQUE,
    created_at    TIMESTAMP   NOT NULL DEFAULT current_timestamp
);

ALTER TABLE sessions
    add user_id bigint;

CREATE INDEX sessions_user_id_index
    ON sessions (user_id);

ALTER TABLE sessions
    ADD CONSTRAINT sessions_users_id_fk
        FOREIGN KEY (user_id) REFERENCES users;
