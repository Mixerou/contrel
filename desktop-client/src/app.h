//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_APP_H
#define DESKTOP_CLIENT_APP_H

#include <imgui.h>

#include "workers.h"

namespace app {
extern ImFont *body_font;
extern ImFont *heading_xl_font;

extern workers::ApiWorker api_worker;
extern workers::WebSocketWorker web_socket_worker;

void InitFonts();

void InitStyle();

namespace states {
struct System {
  enum class Screen {
    kAuth,
    kHotels,
  };

  Screen current_screen;
  bool is_online;

  System();

  std::string GetSessionToken();

  void SetSessionToken(std::string session_token);

  bool IsWebSocketWorkerReadyState();

  void SetWebSocketWorkerReadyState(bool is_ready);

 private:
  std::string session_token_;
  std::mutex session_token_mutex_;

  bool is_web_socket_worker_ready_;
  std::mutex is_web_socket_worker_ready_mutex_;
};

extern System system;
}  // namespace states
}  // namespace app

#endif  // DESKTOP_CLIENT_APP_H
