//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_APP_H
#define DESKTOP_CLIENT_APP_H

#include <optional>
#include <unordered_map>
#include <unordered_set>

#include <imgui.h>
#include <msgpack.hpp>

#include "entities.h"
#include "workers.h"

namespace app {
extern ImFont *body_font;
extern ImFont *heading_xl_font;
extern ImFont *heading_large_font;

extern workers::ApiWorker api_worker;

void InitFonts();

void InitStyle();

namespace states {
struct System {
  enum class Screen {
    kAuth,
    kBookings,
    kDashboard,
    kGuests,
    kHotels,
    kRooms,
  };

  Screen current_screen;
  entities::UserId user_id;
  std::optional<entities::HotelId> opened_hotel_id;
  bool is_online;

  System();

  std::string GetSessionToken();

  void SetSessionToken(const std::string &session_token);

  entities::User GetUser() const;

  std::optional<entities::Hotel> GetHotel() const;

  // This method doesn't make a logout backend request
  void Logout();

 private:
  std::string session_token_;
  std::mutex session_token_mutex_;
};

struct Data {
  std::unordered_map<entities::UserId, entities::User> users;
  std::unordered_map<entities::HotelId, entities::Hotel> hotels;
  std::unordered_map<entities::GuestId, entities::Guest> guests;
  std::unordered_map<entities::RoomId, entities::Room> rooms;
  std::unordered_map<entities::BookingId, entities::Booking> bookings;
  // TODO: maybe it's better to use something unordered
  std::vector<entities::LongTimeGuest> long_time_guests;

  void Clear();
};

extern System system;
extern Data data;
}  // namespace states
}  // namespace app

#endif  // DESKTOP_CLIENT_APP_H
