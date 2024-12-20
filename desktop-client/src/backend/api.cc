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
BackendRequest Login(LoginRequestPayload payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/login", ix::HttpClient::kPost,
                              buffer.data()),
      Layer::kApi);

  return request;
}

BackendRequest Register(RegisterRequestPayload payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/auth/register", ix::HttpClient::kPost,
                              buffer.data()),
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
BackendRequest CreateHotel(CreateHotelRequestPayload payload) {
  msgpack::sbuffer buffer;
  msgpack::pack(buffer, payload);

  BackendRequest request(
      app::api_worker.Enqueue("/hotels", ix::HttpClient::kPost, buffer.data()),
      Layer::kApi);

  return request;
}

BackendRequest GetAllHotels() {
  BackendRequest request(app::api_worker.Enqueue("/hotels"), Layer::kApi);
  return request;
}

CreateGuestRequestPayload::CreateGuestRequestPayload(
    std::string first_name, std::string last_name, int64_t date_of_birth,
    entities::Gender gender, std::string phone_number, std::string email,
    entities::DocumentType document_type, std::string document_number,
    entities::Country document_country, int64_t document_valid_until,
    std::string notes)
    : first_name(std::move(first_name)),
      last_name(std::move(last_name)),
      date_of_birth(date_of_birth),
      gender(static_cast<int16_t>(gender)),
      phone_number(std::move(phone_number)),
      email(std::move(email)),
      document_type(static_cast<int16_t>(document_type)),
      document_number(std::move(document_number)),
      document_country(static_cast<int16_t>(document_country)),
      document_valid_until(document_valid_until),
      notes(std::move(notes)) {}
// Guests
BackendRequest CreateGuest(entities::hotel_id_t hotel_id,
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

BackendRequest GetAllGuests(entities::hotel_id_t hotel_id) {
  BackendRequest request(
      app::api_worker.Enqueue(std::format("/hotels/{}/guests", hotel_id)),
      Layer::kApi);
  return request;
}

}  // namespace backend
