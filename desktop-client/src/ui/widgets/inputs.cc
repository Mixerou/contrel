//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
bool InputText(const char *label, char *buf, size_t buf_size,
               ImGuiInputTextFlags flags, ImGuiInputTextCallback callback,
               void *user_data) {
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, kStyleInputPadding);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kStyleInputRounding);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, kColorNeutral200);

  bool input =
      ImGui::InputText(label, buf, buf_size, flags, callback, user_data);

  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);

  return input;
}

bool MetaInputText(const char *label, char *buf, size_t buf_size,
                   ImGuiInputTextFlags flags, ImGuiInputTextCallback callback,
                   void *user_data) {
  ImGui::PushID(label);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0, 4.0));

  ImGui::Indent(kStyleInputRounding);
  widgets::BodyText("%s", label);
  ImGui::Unindent(kStyleInputRounding);

  bool input = InputText("", buf, buf_size, flags, callback, user_data);

  ImGui::PopStyleVar();
  ImGui::PopID();

  return input;
}
}  // namespace widgets
