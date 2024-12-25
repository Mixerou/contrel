CREATE TABLE "bookings"
(
    id           BIGINT    NOT NULL PRIMARY KEY,
    hotel_id     BIGINT    NOT NULL,
    room_id      BIGINT    NOT NULL,
    check_in_at  TIMESTAMP NOT NULL,
    check_out_at TIMESTAMP NOT NULL,
    created_at   TIMESTAMP NOT NULL default current_timestamp,
    FOREIGN KEY (hotel_id) REFERENCES "hotels" (id) ON DELETE RESTRICT,
    FOREIGN KEY (room_id) REFERENCES "rooms" (id) ON DELETE RESTRICT
);

-- It would be nice to add a check to prevent guests from different hotels from being added to single bookings.
CREATE TABLE "booking_guests"
(
    id         BIGINT NOT NULL PRIMARY KEY,
    booking_id BIGINT NOT NULL,
    guest_id   BIGINT NOT NULL,
    FOREIGN KEY (booking_id) REFERENCES bookings (id) ON DELETE CASCADE,
    FOREIGN KEY (guest_id) REFERENCES guests (id) ON DELETE RESTRICT
);
