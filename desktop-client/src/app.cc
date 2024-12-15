//
// Created by Mixerou on 14.12.2024.
//

#include <imgui.h>

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

  style.Colors[ImGuiCol_Text] = kColorDefaultText;
  style.Colors[ImGuiCol_WindowBg] = kColorDefaultBackground;

  style.Colors[ImGuiCol_NavCursor] = ImVec4(0.0, 0.0, 0.0, 0.0);
}
}  // namespace app
