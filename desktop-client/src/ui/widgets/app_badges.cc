//
// Created by Mixerou on 15.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
void ErrorAppBadge(const char *fmt, const ColorAccent color_accent) {
  auto viewport_size = ImGui::GetMainViewport()->WorkSize;

  switch (color_accent) {
    case ColorAccent::kDanger:
      ImGui::PushStyleColor(ImGuiCol_WindowBg, kColorDanger100);
      ImGui::PushStyleColor(ImGuiCol_Text, kColorDanger700);
      break;
    default:
      ImGui::PushStyleColor(ImGuiCol_WindowBg, kColorPrimary100);
      ImGui::PushStyleColor(ImGuiCol_Text, kColorPrimary700);
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0, 8.0));
  ImGui::SetNextWindowPos(
      ImVec2(viewport_size.x - 16.0, viewport_size.y - 16.0), ImGuiCond_Always,
      ImVec2(1.0, 1.0));
  ImGui::Begin(fmt, nullptr, kWindowFloatFlags);

  BodyText(fmt);

  ImGui::End();
  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor(2);
}
}  // namespace widgets
