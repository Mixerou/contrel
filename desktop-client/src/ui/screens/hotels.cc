//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "app.h"
#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

namespace screens {
void HotelsScreen() {
  layouts::BeginAppLayout("Yor Hotels | WiP");

  auto available_region = ImGui::GetContentRegionAvail();
  auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
  auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                              viewport_work_size.y - available_region.y);

  ImGui::SetNextWindowPos(
      ImVec2(viewport_work_size.x / 2.0 + occupied_size.x / 2.0,
             viewport_work_size.y / 2.0 + occupied_size.y / 2.0),
      ImGuiCond_Always, ImVec2(0.5, 0.5));
  ImGui::BeginChild("hotels", ImVec2(0.0, 0.0), kChildWindowFitContent);

  auto user_first_name = app::states::system.GetUser().first_name;
  widgets::HeadingXlTextCenter("Welcome to Contrel, %s",
                               user_first_name.c_str());

  ImGui::EndChild();

  layouts::EndAppLayout();
}
}  // namespace screens
