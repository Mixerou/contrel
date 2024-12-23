//
// Created by Mixerou on 18.12.2024.
//

#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

namespace screens {
void DashboardScreen() {
  layouts::BeginAppLayout("Summary and Nerd Stats");

  const auto available_region = ImGui::GetContentRegionAvail();
  const auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
  const auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                                    viewport_work_size.y - available_region.y);

  ImGui::SetNextWindowPos(
      ImVec2(viewport_work_size.x / 2.f + occupied_size.x / 2.f,
             viewport_work_size.y / 2.f + occupied_size.y / 2.f),
      ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::BeginChild("info", ImVec2(), kChildWindowFitContent);
  widgets::HeadingXlTextCenter("There is nothing to check");
  ImGui::EndChild();

  layouts::EndAppLayout();
}
}  // namespace screens
