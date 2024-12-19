CREATE TABLE "guests"
(
    id                   BIGINT PRIMARY KEY,
    first_name           VARCHAR(32)  NOT NULL,
    last_name            VARCHAR(32)  NOT NULL,
    date_of_birth        TIMESTAMP    NOT NULL,
    gender               SMALLINT     NOT NULL,
    phone_number         VARCHAR(16)  NOT NULL,
    email                VARCHAR(64)  NOT NULL,
    document_type        SMALLINT     NOT NULL,
    document_number      VARCHAR(16)  NOT NULL,
    document_country     BIGINT       NOT NULL,
    document_valid_until TIMESTAMP    NOT NULL,
    notes                VARCHAR(128) NOT NULL,
    hotel_id             BIGINT       NOT NULL,
    created_at           TIMESTAMP    NOT NULL DEFAULT current_timestamp,
    UNIQUE (phone_number, hotel_id),
    UNIQUE (email, hotel_id),
    UNIQUE (document_type, document_number, document_country, document_valid_until, hotel_id),
    FOREIGN KEY (hotel_id) REFERENCES "hotels" (id) ON DELETE RESTRICT
);
