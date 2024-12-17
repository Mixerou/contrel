//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

namespace screens {
void HotelsScreen() {
  layouts::BeginAppLayout();

  auto center = ImGui::GetMainViewport()->GetWorkCenter();

  ImGui::SetNextWindowPos(ImVec2(center.x, center.y), ImGuiCond_Always,
                          ImVec2(0.5, 0.5));
  ImGui::BeginChild("hotels", ImVec2(0.0, 0.0), kChildWindowFitContent);

  widgets::HeadingXlTextCenter("Welcome to Contrel");

  ImGui::EndChild();

  layouts::EndAppLayout();
}
}  // namespace screens
