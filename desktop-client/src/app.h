//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_APP_H
#define DESKTOP_CLIENT_APP_H

#include <optional>
#include <unordered_map>

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
    kDashboard,
    kHotels,
  };

  Screen current_screen;
  std::optional<entities::hotel_id_t> opened_hotel_id;
  entities::user_id_t user_id;
  bool is_online;

  System();

  std::string GetSessionToken();

  void SetSessionToken(std::string session_token);

  entities::User GetUser();

  // This method doesn't make a logout backend request
  void Logout();

 private:
  std::string session_token_;
  std::mutex session_token_mutex_;
};

struct Data {
  std::unordered_map<entities::user_id_t, entities::User> users;
  std::unordered_map<entities::hotel_id_t, entities::Hotel> hotels;

  void Clear();
};

extern System system;
extern Data data;
}  // namespace states
}  // namespace app

#endif  // DESKTOP_CLIENT_APP_H
