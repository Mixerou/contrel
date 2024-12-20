//
// Created by Mixerou on 20.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
bool BeginCombo(const char* label, const char* preview_value,
                const ImGuiComboFlags flags) {
  ImGui::PushID(label);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0, 4.0));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, kStyleComboPadding);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kStyleComboRounding);

  ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0);
  ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, kStyleComboPopupRounding);

  ImGui::PushStyleColor(ImGuiCol_FrameBg, kColorNeutral200);
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, kColorNeutral300);
  ImGui::PushStyleColor(ImGuiCol_FrameBgActive, kColorNeutral400);

  ImGui::PushStyleColor(ImGuiCol_Header, kColorNeutral300);
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, kColorNeutral300);
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, kColorNeutral400);

  ImGui::PushStyleColor(ImGuiCol_PopupBg, kColorNeutral200);

  ImGui::Indent(kStyleComboRounding);
  BodyText("%s", label);
  ImGui::Unindent(kStyleComboRounding);

  return ImGui::BeginCombo("", preview_value,
                           flags | ImGuiComboFlags_NoArrowButton);
}

void EndCombo(const bool begin_combo_state) {
  if (begin_combo_state) ImGui::EndCombo();

  ImGui::PopStyleVar(5);
  ImGui::PopStyleColor(7);
  ImGui::PopID();
}
}  // namespace widgets
