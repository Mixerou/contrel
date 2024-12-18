//
// Created by Mixerou on 17.12.2024.
//

#include <format>

#include <imgui.h>

#include "app.h"
#include "backend.h"
#include "constants.h"
#include "layouts.h"
#include "widgets.h"

using namespace constants;

void AppBar(const char *screen_heading, const float &height,
            const float &nav_bar_width) {
  static bool is_requesting = false;
  static backend::BackendRequest request;
  bool is_logout_clicked = false;

  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0, 1.0, 1.0, 1.0));
  ImGui::SetNextWindowPos(ImVec2(nav_bar_width, 0.0), ImGuiCond_Always,
                          ImVec2(0.0, 0.0));
  ImGui::SetNextWindowSize(ImVec2(viewport_size.x - nav_bar_width, height));
  ImGui::BeginChild("app_layout_app_bar");

  const auto app_bar_size = ImGui::GetContentRegionAvail();

  // Left Side
  {
    auto heading_size = widgets::CalculateHeadingLargeText(screen_heading);
    auto heading_position = ImVec2(kStyleScreenPadding.x,
                                   app_bar_size.y / 2.0 - heading_size.y / 2.0);

    ImGui::SetCursorPos(heading_position);
    widgets::HeadingLargeText(screen_heading);
  }

  // Right Side
  {
    auto draw = ImGui::GetWindowDrawList();

    auto user = app::states::system.GetUser();
    std::string user_name =
        std::format("{} {}", user.first_name, user.last_name);
    auto user_name_size = widgets::CalculateBodyText(user_name.c_str());
    auto button_size = ImVec2(user_name_size.x + kStyleButtonPadding.x * 2,
                              user_name_size.y + kStyleButtonPadding.y * 2);
    auto button_start_position =
        ImVec2(app_bar_size.x - kStyleScreenPadding.x - button_size.x,
               app_bar_size.y / 2.0 - button_size.y / 2.0);
    auto button_end_position =
        ImVec2(app_bar_size.x - kStyleScreenPadding.x,
               app_bar_size.y / 2.0 + button_size.y / 2.0);

    if (ImGui::IsMouseHoveringRect(button_start_position,
                                   button_end_position)) {
      ImGui::SetCursorPos(
          ImVec2(button_start_position.x, button_start_position.y));
      is_logout_clicked = widgets::Button("Logout", button_size, false,
                                          widgets::ColorAccent::kDanger);
    } else {
      draw->AddRectFilled(button_start_position, button_end_position,
                          ImGui::ColorConvertFloat4ToU32(kColorNeutral200),
                          kStyleButtonRounding);

      ImGui::SetCursorPos(
          ImVec2(button_start_position.x + kStyleButtonPadding.x,
                 button_start_position.y + kStyleButtonPadding.y));
      widgets::BodyText(user_name.c_str());
    }
  }

  if (is_logout_clicked) {
    is_requesting = true;
    request = backend::Logout();
  }

  if (is_requesting) {
    backend::EmptyResponse empty_response;
    auto response = backend::GetResponse(request, empty_response);

    if (response == backend::ResponseStatus::kCompleted)
      app::states::system.Logout();

    if (response != backend::ResponseStatus::kInProcess) is_requesting = false;
  }

  ImGui::PopStyleColor();
  ImGui::EndChild();
}

namespace layouts {
void BeginAppLayout(const char *screen_heading) {
  ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
  ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
  ImGui::Begin("app_layout", nullptr, kWindowDefaultFlags);

  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;
  const float nav_bar_width = 0.0;
  const float app_bar_height = 56.0;

  AppBar(screen_heading, app_bar_height, nav_bar_width);

  // Screen Content
  {
    ImGui::SetNextWindowPos(ImVec2(nav_bar_width, app_bar_height),
                            ImGuiCond_Always, ImVec2(0.0, 0.0));
    ImGui::SetNextWindowSize(ImVec2(viewport_size.x - nav_bar_width,
                                    viewport_size.y - app_bar_height));

    ImGui::BeginChild("app_layout_screen");
  }
}

void EndAppLayout() {
  ImGui::EndChild();
  ImGui::End();
}
}  // namespace layouts
