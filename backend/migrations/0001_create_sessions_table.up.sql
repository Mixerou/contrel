CREATE TABLE "sessions"
(
    id         BIGINT PRIMARY KEY,
    token      VARCHAR(96) NOT NULL UNIQUE,
    created_at TIMESTAMP   NOT NULL DEFAULT current_timestamp
);
