//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "app.h"
#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
bool Button(const char *label, const ImVec2 &size, const bool is_disabled,
            const ColorAccent color_accent) {
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, kStyleButtonPadding);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kStyleButtonRounding);

  if (!is_disabled && app::states::system.is_online) switch (color_accent) {
      case ColorAccent::kDanger:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorDanger500);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorDanger600);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorDanger700);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
        break;
      case ColorAccent::kPrimaryLight:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorPrimary50);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorPrimary100);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorPrimary200);
        ImGui::PushStyleColor(ImGuiCol_Text, kColorPrimary600);
        break;
      case ColorAccent::kPrimaryBlank:
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 0.0, 0.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorPrimary50);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorPrimary100);
        ImGui::PushStyleColor(ImGuiCol_Text, kColorNeutral600);
        break;
      default:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorPrimary500);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorPrimary600);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorPrimary700);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
    }
  else
    switch (color_accent) {
      case ColorAccent::kDanger:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorDanger700);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorDanger700);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorDanger700);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
        break;
      case ColorAccent::kPrimaryLight:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorPrimary200);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorPrimary200);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorPrimary200);
        ImGui::PushStyleColor(ImGuiCol_Text, kColorPrimary600);
        break;
      case ColorAccent::kPrimaryBlank:
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0, 0.0, 0.0, 0.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                              ImVec4(0.0, 0.0, 0.0, 0.0));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                              ImVec4(0.0, 0.0, 0.0, 0.0));
        ImGui::PushStyleColor(ImGuiCol_Text, kColorNeutral400);
        break;
      default:
        ImGui::PushStyleColor(ImGuiCol_Button, kColorPrimary700);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, kColorPrimary700);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, kColorPrimary700);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
    }

  bool button = ImGui::Button(label, size);

  ImGui::PopStyleColor(4);
  ImGui::PopStyleVar(2);

  return !is_disabled && app::states::system.is_online && button;
}
}  // namespace widgets
