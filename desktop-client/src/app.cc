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

workers::ApiWorker api_worker;

void InitFonts() {
  auto io = ImGui::GetIO();

  body_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/nunito-sans-regular.ttf", 18.0 * kFontSizeFactor, nullptr,
      io.Fonts->GetGlyphRangesCyrillic());
  body_font->Scale = kFontScaleFactor;
  IM_ASSERT(body_font != nullptr);

  heading_xl_font = io.Fonts->AddFontFromFileTTF(
      "assets/fonts/nunito-sans-semi-bold.ttf", 32.0 * kFontSizeFactor, nullptr,
      io.Fonts->GetGlyphRangesCyrillic());
  heading_xl_font->Scale = kFontScaleFactor;
  IM_ASSERT(heading_xl_font != nullptr);

  io.Fonts->Build();
}

void InitStyle() {
  auto &style = ImGui::GetStyle();

  style.WindowPadding = ImVec2(0.0, 0.0);
  style.WindowBorderSize = 0.0;
  style.ChildBorderSize = 0.0;
  style.FrameBorderSize = 0.0;

  style.ItemSpacing = kStyleItemSpacing;

  style.Colors[ImGuiCol_Text] = kColorDefaultText;
  style.Colors[ImGuiCol_WindowBg] = kColorDefaultBackground;

  style.Colors[ImGuiCol_NavCursor] = ImVec4(0.0, 0.0, 0.0, 0.0);
}
}  // namespace app

namespace app::states {
System::System() : is_online(true), current_screen(Screen::kAuth) {
  keychain::Error error;
  std::lock_guard<std::mutex> lock(session_token_mutex_);

  session_token_ =
      keychain::getPassword("dev.mixero.contrel", "desktop-client", "", error);
}

std::string System::GetSessionToken() {
  std::lock_guard<std::mutex> lock(session_token_mutex_);
  return session_token_;
}

void System::SetSessionToken(std::string session_token) {
  {
    std::lock_guard<std::mutex> lock(session_token_mutex_);
    session_token_ = session_token;
  }

  keychain::Error error;
  keychain::setPassword("dev.mixero.contrel", "desktop-client", "",
                        session_token, error);
}

void System::Logout() {
  current_screen = app::states::System::Screen::kAuth;
  user_id = 0;
  data.Clear();
}

System system;
}  // namespace app::states

namespace app::states {
void Data::Clear() { users.clear(); }

Data data;
}  // namespace app::states
