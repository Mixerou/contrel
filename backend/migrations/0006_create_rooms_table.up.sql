CREATE TABLE "rooms"
(
    id         BIGINT PRIMARY KEY,
    number     VARCHAR(32) NOT NULL,
--     This should be a foreign key to the `room_groups` table, but...
    group_name VARCHAR(32) NOT NULL,
    hotel_id   BIGINT      NOT NULL,
    created_at TIMESTAMP   NOT NULL DEFAULT current_timestamp,
    UNIQUE (number, group_name, hotel_id),
    FOREIGN KEY (hotel_id) REFERENCES "hotels" (id) ON DELETE RESTRICT
);
