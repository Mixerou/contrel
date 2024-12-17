//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "backend.h"
#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

namespace screens {
void HotelsScreen() {
  layouts::BeginAppLayout();

  const auto center = ImGui::GetMainViewport()->GetWorkCenter();
  static bool is_requesting = false;
  static backend::BackendRequest request;

  ImGui::SetNextWindowPos(ImVec2(center.x, center.y), ImGuiCond_Always,
                          ImVec2(0.5, 0.5));
  ImGui::BeginChild("hotels", ImVec2(0.0, 0.0), kChildWindowFitContent);

  widgets::HeadingXlTextCenter("Welcome to Contrel");

  const auto available_region = ImGui::GetContentRegionAvail();
  const float button_width =
      ImGui::CalcTextSize("Logout").x + kStyleButtonPadding.x * 2;
  ImGui::SetCursorPosX(available_region.x / 2.0 - button_width / 2.0);
  auto logout_button = widgets::Button(
      "Logout", ImVec2(0.0, 0.0), is_requesting, widgets::ColorAccent::kDanger);

  if (logout_button) {
    is_requesting = true;
    request = backend::Logout();
  }

  if (is_requesting) {
    backend::EmptyResponse empty_response;
    auto response = backend::GetResponse(request, empty_response);

    if (response == backend::ResponseStatus::kCompleted) {
      is_requesting = false;
      app::states::system.current_screen = app::states::System::Screen::kAuth;
    } else if (response != backend::ResponseStatus::kInProcess) {
      is_requesting = false;
    }
  }

  ImGui::EndChild();

  layouts::EndAppLayout();
}
}  // namespace screens
