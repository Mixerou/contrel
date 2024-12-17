//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "layouts.h"

using namespace constants;

namespace layouts {
void BeginAppLayout() {
  ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
  ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
  ImGui::Begin("app_layout", nullptr, kWindowDefaultFlags);

  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;

  // Screen Content
  {
    ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_Always,
                            ImVec2(0.0, 0.0));
    ImGui::SetNextWindowSize(ImVec2(viewport_size.x, viewport_size.y));

    ImGui::BeginChild("app_layout_screen");
  }
}

void EndAppLayout() {
  ImGui::EndChild();
  ImGui::End();
}
}  // namespace layouts
