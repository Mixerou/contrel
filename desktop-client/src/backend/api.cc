//
// Created by Mixerou on 15.12.2024.
//

#include <ixwebsocket/IXHttpClient.h>

#include "app.h"
#include "backend.h"
#include "entities.h"

namespace backend {
// Ping
BackendRequest Ping() {
  BackendRequest request(app::api_worker.Enqueue("/ping"), Layer::kApi);
  return request;
}

// Auth
BackendRequest Login(const LoginRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/login", ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest Register(const RegisterRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/register", ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest Logout() {
  BackendRequest request(
      app::api_worker.Enqueue("/auth/logout", ix::HttpClient::kPost),
      Layer::kApi);
  return request;
}

// Users
BackendRequest GetMe() {
  BackendRequest request(app::api_worker.Enqueue("/users/@me"), Layer::kApi);
  return request;
}

// Hotels
BackendRequest CreateHotel(const CreateHotelRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/hotels", ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest GetAllHotels() {
  BackendRequest request(app::api_worker.Enqueue("/hotels"), Layer::kApi);
  return request;
}

// Guests
BackendRequest CreateGuest(entities::HotelId hotel_id,
                           const CreateGuestRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/guests", hotel_id),
                              ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest GetAllGuests(const entities::HotelId hotel_id) {
  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/guests", hotel_id)),
      Layer::kApi);
  return request;
}

// Rooms
BackendRequest CreateRoom(entities::HotelId hotel_id,
                          const CreateRoomRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/rooms", hotel_id),
                              ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest GetAllRooms(const entities::HotelId hotel_id) {
  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/rooms", hotel_id)),
      Layer::kApi);
  return request;
}

// Bookings
BackendRequest CreateBooking(entities::HotelId hotel_id,
                             const CreateBookingRequestPayload &payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/bookings", hotel_id),
                              ix::HttpClient::kPost,
                              std::string(buffer.data(), buffer.size())),
      Layer::kApi);

  return request;
}

BackendRequest GetAllBookings(const entities::HotelId hotel_id) {
  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/bookings", hotel_id)),
      Layer::kApi);
  return request;
}

// Stats
BackendRequest GetStatsLongTimeGuests(const entities::HotelId hotel_id) {
  BackendRequest request(app::api_worker.Enqueue(std::format(
                             "/hotels/{}/stats/long-time-guests", hotel_id)),
                         Layer::kApi);
  return request;
}
}  // namespace backend
