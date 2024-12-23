//
// Created by Mixerou on 14.12.2024.
//

#include <iostream>

#include <imgui.h>
#include <keychain/keychain.h>

#include "app.h"
#include "constants.h"
#include "workers.h"

using namespace constants;

namespace app {
ImFont *body_font = nullptr;
ImFont *heading_xl_font = nullptr;
ImFont *heading_large_font = nullptr;

workers::ApiWorker api_worker;

void InitFonts() {
  const auto io = ImGui::GetIO();

  body_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/nunito-sans-regular.ttf", 18.f * kFontSizeFactor, nullptr,
      io.Fonts->GetGlyphRangesCyrillic());
  body_font->Scale = kFontScaleFactor;
  IM_ASSERT(body_font != nullptr);

  heading_xl_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/nunito-sans-semi-bold.ttf", 32.f * kFontSizeFactor, nullptr,
      io.Fonts->GetGlyphRangesCyrillic());
  heading_xl_font->Scale = kFontScaleFactor;
  IM_ASSERT(heading_xl_font != nullptr);

  heading_large_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/nunito-sans-semi-bold.ttf", 28.f * kFontSizeFactor, nullptr,
      io.Fonts->GetGlyphRangesCyrillic());
  heading_large_font->Scale = kFontScaleFactor;
  IM_ASSERT(heading_large_font != nullptr);

  io.Fonts->Build();
}

void InitStyle() {
  auto &style = ImGui::GetStyle();

  style.WindowPadding = ImVec2();
  style.WindowBorderSize = 0.f;
  style.ChildBorderSize = 0.f;

  style.FrameBorderSize = 0.f;
  style.FrameRounding = 16.f;

  style.ItemSpacing = kStyleItemSpacing;

  style.CellPadding.x = 0.f;

  style.Colors[ImGuiCol_Text] = kColorDefaultText;
  style.Colors[ImGuiCol_WindowBg] = kColorDefaultBackground;

  style.Colors[ImGuiCol_Header] = ImVec4();
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4();
  style.Colors[ImGuiCol_HeaderActive] = ImVec4();

  style.Colors[ImGuiCol_TableHeaderBg] = ImVec4();
  style.Colors[ImGuiCol_TableRowBg] = ImVec4();
  style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4();
  style.Colors[ImGuiCol_TableBorderStrong] = ImVec4();
  style.Colors[ImGuiCol_TableBorderLight] = ImVec4();

  style.Colors[ImGuiCol_NavCursor] = ImVec4();
}
}  // namespace app

namespace app::states {
System::System()
    : current_screen(Screen::kAuth), user_id(0.f), is_online(true) {
  keychain::Error error;
  std::lock_guard lock(session_token_mutex_);

  session_token_ =
      getPassword("dev.mixero.contrel", "desktop-client", "", error);
}

std::string System::GetSessionToken() {
  std::lock_guard lock(session_token_mutex_);
  return session_token_;
}

void System::SetSessionToken(const std::string &session_token) {
  {
    std::lock_guard lock(session_token_mutex_);
    session_token_ = session_token;
  }

  keychain::Error error;
  setPassword("dev.mixero.contrel", "desktop-client", "", session_token, error);
}

entities::User System::GetUser() const { return data.users[user_id]; }

std::optional<entities::Hotel> System::GetHotel() const {
  if (opened_hotel_id.has_value()) return data.hotels[opened_hotel_id.value()];

  return std::nullopt;
}

void System::Logout() {
  opened_hotel_id = std::nullopt;
  current_screen = Screen::kAuth;
  user_id = 0;
  data.Clear();
}

System system;
}  // namespace app::states

namespace app::states {
void Data::Clear() {
  users.clear();
  hotels.clear();
}

Data data;
}  // namespace app::states
