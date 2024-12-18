CREATE TABLE "hotels"
(
    id         BIGINT PRIMARY KEY,
    name       VARCHAR(64) NOT NULL,
    owner_id   BIGINT      NOT NULL,
    created_at TIMESTAMP   NOT NULL DEFAULT current_timestamp,
    UNIQUE (name, owner_id),
    FOREIGN KEY (owner_id) REFERENCES "users" (id) ON DELETE RESTRICT
);
