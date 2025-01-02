//
// Created by Mixerou on 15.12.2024.
// This is the backend transport layer,
// not the backend itself.
//

#ifndef DESKTOP_CLIENT_BACKEND_H
#define DESKTOP_CLIENT_BACKEND_H

#include <iostream>
#include <utility>
#include <vector>

#include <msgpack.hpp>

#include "app.h"
#include "entities.h"

namespace backend {
enum class Layer {
  kApi,
};

struct ErrorResponse {
  int code;
  std::string message;

  ErrorResponse() : code(0) {}
  MSGPACK_DEFINE(code, message);
};

struct BackendRequest {
  int worker_request_id;
  Layer layer;
  ErrorResponse error_response;

  BackendRequest() : worker_request_id(-1), layer(Layer::kApi) {}

  BackendRequest(const int worker_request_id, const Layer layer)
      : worker_request_id(worker_request_id), layer(layer) {}
};

enum class ResponseStatus {
  kInProcess,
  kCompleted,
  kError,
  kCompetedWithError,
};

// In MessagePack this is a `90`
struct EmptyResponse {
  EmptyResponse() {}
  MSGPACK_DEFINE();
};

// Ping
typedef std::string PingResponse;

BackendRequest Ping();

// Login
struct LoginRequestPayload {
  std::string email;
  std::string password;

  MSGPACK_DEFINE(email, password);
};

BackendRequest Login(const LoginRequestPayload &payload);

// Register
struct RegisterRequestPayload {
  std::string email;
  std::string password;
  std::string first_name;
  std::string last_name;

  MSGPACK_DEFINE(email, password, first_name, last_name);
};

BackendRequest Register(const RegisterRequestPayload &payload);

// Logout
BackendRequest Logout();

// Get Me
typedef entities::User GetMeResponse;

BackendRequest GetMe();

// Create Hotel
struct CreateHotelRequestPayload {
  std::string name;

  MSGPACK_DEFINE(name);
};

typedef entities::Hotel CreateHotelResponse;

BackendRequest CreateHotel(const CreateHotelRequestPayload &payload);

// Get All Hotels
typedef std::vector<entities::Hotel> GetAllHotelsResponse;

BackendRequest GetAllHotels();

// For Guest Requests
struct GetGuestResponse {
  entities::GuestId id;
  std::string first_name;
  std::string last_name;
  int64_t date_of_birth;
  int16_t gender;
  std::string phone_number;
  std::string email;
  int16_t document_type;
  std::string document_number;
  int16_t document_country;
  int64_t document_valid_until;
  std::string notes;
  entities::HotelId hotel_id;
  int64_t created_at;

  entities::Guest ToGuest() const {
    return entities::Guest{id,
                           first_name,
                           last_name,
                           date_of_birth,
                           static_cast<entities::Gender>(gender),
                           phone_number,
                           email,
                           static_cast<entities::DocumentType>(document_type),
                           document_number,
                           static_cast<entities::Country>(document_country),
                           document_valid_until,
                           notes,
                           hotel_id,
                           created_at};
  }

  MSGPACK_DEFINE(id, first_name, last_name, date_of_birth, gender, phone_number,
                 email, document_type, document_number, document_country,
                 document_valid_until, notes, hotel_id, created_at);
};

// Create Guest
struct CreateGuestRequestPayload {
  std::string first_name;
  std::string last_name;
  int64_t date_of_birth;
  int16_t gender;
  std::string phone_number;
  std::string email;
  int16_t document_type;
  std::string document_number;
  int16_t document_country;
  int64_t document_valid_until;
  std::string notes;

  CreateGuestRequestPayload(std::string first_name, std::string last_name,
                            int64_t date_of_birth, entities::Gender gender,
                            std::string phone_number, std::string email,
                            entities::DocumentType document_type,
                            std::string document_number,
                            entities::Country document_country,
                            int64_t document_valid_until, std::string notes)
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

  MSGPACK_DEFINE(first_name, last_name, date_of_birth, gender, phone_number,
                 email, document_type, document_number, document_country,
                 document_valid_until, notes);
};

typedef GetGuestResponse CreateGuestResponse;

BackendRequest CreateGuest(entities::HotelId hotel_id,
                           const CreateGuestRequestPayload &payload);

// Get All Guests
typedef std::vector<GetGuestResponse> GetAllGuestsResponse;

BackendRequest GetAllGuests(entities::HotelId hotel_id);

// Create Room
struct CreateRoomRequestPayload {
  std::string number;
  std::string group_name;

  MSGPACK_DEFINE(number, group_name);
};

typedef entities::Room CreateRoomResponse;

BackendRequest CreateRoom(entities::HotelId hotel_id,
                          const CreateRoomRequestPayload &payload);

// Get All Rooms
typedef std::vector<entities::Room> GetAllRoomsResponse;

BackendRequest GetAllRooms(entities::HotelId hotel_id);

// Create Booking
struct CreateBookingRequestPayload {
  entities::RoomId room_id;
  int64_t check_in_at;
  int64_t check_out_at;
  std::vector<entities::GuestId> guest_ids;

  MSGPACK_DEFINE(room_id, check_in_at, check_out_at, guest_ids);
};

typedef entities::Booking CreateBookingResponse;

BackendRequest CreateBooking(entities::HotelId hotel_id,
                             const CreateBookingRequestPayload &payload);

// Get All Bookings
typedef std::vector<entities::Booking> GetAllBookingsResponse;

BackendRequest GetAllBookings(entities::HotelId hotel_id);

// Get Stats Long-Time Guests
typedef std::vector<entities::LongTimeGuest> GetStatsLongTimeGuestsResponse;

BackendRequest GetStatsLongTimeGuests(entities::HotelId hotel_id);

template <typename T>
ResponseStatus GetResponse(BackendRequest &request, T &response_reference) {
  switch (request.layer) {
    case Layer::kApi: {
      auto response = app::api_worker.GetResponse(request.worker_request_id);

      if (!response.has_value()) return ResponseStatus::kInProcess;

      auto unwrapped_response = response->get();
      auto body = unwrapped_response->body;

      if (unwrapped_response->errorCode != ix::HttpErrorCode::Ok)
        return ResponseStatus::kError;

      if (body.empty()) return ResponseStatus::kCompleted;

      auto object = msgpack::unpack(body.c_str(), body.size()).get();

      // TODO: catch msgpack::type_error
      try {
        object.convert(request.error_response);
        return ResponseStatus::kCompetedWithError;
      } catch (...) {
        try {
          object.convert(response_reference);
          return ResponseStatus::kCompleted;
        } catch (...) {
          return ResponseStatus::kError;
        }
      }
    }
    default:
      // theoretically unreachable code
      return ResponseStatus::kError;
  }
}
}  // namespace backend

#endif  // DESKTOP_CLIENT_BACKEND_H
