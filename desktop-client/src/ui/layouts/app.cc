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

static bool is_prepare_unmount = false;
static std::string last_screen_heading;

void AppBar(const char *screen_heading, const float &height,
            const float &nav_bar_width) {
  static bool is_requesting = false;
  static backend::BackendRequest request;
  bool is_logout_clicked = false;

  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 1.f));
  ImGui::SetNextWindowPos(ImVec2(nav_bar_width, 0.f), ImGuiCond_Always,
                          ImVec2());
  ImGui::SetNextWindowSize(ImVec2(viewport_size.x - nav_bar_width, height));
  ImGui::BeginChild("app_layout_app_bar");

  // Left Side
  {
    const auto heading_size =
        widgets::CalculateHeadingLargeText(screen_heading);
    const auto heading_position =
        ImVec2(kStyleScreenPadding.x, height / 2.f - heading_size.y / 2.f);

    ImGui::SetCursorPos(heading_position);
    widgets::HeadingLargeText(screen_heading);
  }

  // Right Side
  {
    const auto draw = ImGui::GetWindowDrawList();

    auto user = app::states::system.GetUser();
    const std::string user_name =
        std::format("{} {}", user.first_name, user.last_name);
    const auto user_name_size = widgets::CalculateBodyText(user_name.c_str());
    const auto button_size =
        ImVec2(user_name_size.x + kStyleButtonPadding.x * 2.f,
               user_name_size.y + kStyleButtonPadding.y * 2.f);
    const auto button_start_position =
        ImVec2(viewport_size.x - kStyleScreenPadding.x - button_size.x,
               height / 2.f - button_size.y / 2.f);
    const auto button_end_position =
        ImVec2(viewport_size.x - kStyleScreenPadding.x,
               height / 2.f + button_size.y / 2.f);

    if (ImGui::IsMouseHoveringRect(button_start_position,
                                   button_end_position)) {
      ImGui::SetCursorScreenPos(
          ImVec2(button_start_position.x, button_start_position.y));
      is_logout_clicked =
          Button("Logout", button_size, false, widgets::ColorAccent::kDanger);
    } else {
      draw->AddRectFilled(button_start_position, button_end_position,
                          ImGui::ColorConvertFloat4ToU32(kColorNeutral200),
                          kStyleButtonRounding);

      ImGui::SetCursorScreenPos(
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
    const auto response = GetResponse(request, empty_response);

    if (response == backend::ResponseStatus::kCompleted) {
      is_prepare_unmount = true;
      app::states::system.Logout();
    }

    if (response != backend::ResponseStatus::kInProcess) is_requesting = false;
  }

  ImGui::PopStyleColor();
  ImGui::EndChild();
}

void NavBar(const float width) {
  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 1.f));
  ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.f, 0.5f));
  ImGui::SetNextWindowPos(ImVec2(), ImGuiCond_Always, ImVec2());
  ImGui::BeginChild("app_layout_nav_bar", ImVec2(width, viewport_size.y));

  const auto available_region = ImGui::GetContentRegionAvail();
  const float button_width = available_region.x - kStyleScreenPadding.x * 2.f;

  // Hotel Top Button
  const auto hotel = app::states::system.GetHotel();
  const auto draw = ImGui::GetWindowDrawList();

  if (hotel.has_value()) {
    const auto hotel_name_size =
        widgets::CalculateBodyText(hotel->name.c_str());
    const auto button_size =
        ImVec2(button_width, hotel_name_size.y + kStyleButtonPadding.y * 2.f);
    const auto button_start_position = ImVec2(kStyleScreenPadding.x, 12.f);
    const auto button_end_position = ImVec2(
        available_region.x - kStyleScreenPadding.x, 12.f + button_size.y);

    if (ImGui::IsMouseHoveringRect(button_start_position,
                                   button_end_position)) {
      ImGui::SetCursorScreenPos(
          ImVec2(button_start_position.x, button_start_position.y));
      const auto is_hotels_button =
          widgets::Button("Back to hotels", button_size);

      if (is_hotels_button) {
        app::states::system.current_screen =
            app::states::System::Screen::kHotels;
        app::states::system.opened_hotel_id = std::nullopt;
      }
    } else {
      //       I don't really like any of this
      draw->AddRect(button_start_position, button_end_position,
                    ImGui::ColorConvertFloat4ToU32(ImVec4()),
                    kStyleButtonRounding);
      //      draw->AddRect(button_start_position, button_end_position,
      //                          ImGui::ColorConvertFloat4ToU32(kColorPrimary500),
      //                          kStyleButtonRounding);

      ImGui::PushStyleColor(ImGuiCol_Text, kColorPrimary500);
      ImGui::SetCursorScreenPos(
          ImVec2(button_start_position.x + kStyleButtonPadding.x,
                 button_start_position.y + kStyleButtonPadding.y));
      widgets::BodyText(hotel->name.c_str());
      ImGui::PopStyleColor();

      ImGui::Spacing();
    }
  }

  ImGui::Spacing();
  ImGui::Spacing();

  // Navigation Buttons
  {
    const auto current_screen = app::states::system.current_screen;

    ImGui::SetCursorPosX(kStyleScreenPadding.x);
    const auto is_dashboard_button =
        Button("Dashboard", ImVec2(button_width, 0.f), false,
               current_screen == app::states::System::Screen::kDashboard
                   ? widgets::ColorAccent::kPrimaryLight
                   : widgets::ColorAccent::kPrimaryBlank);

    if (is_dashboard_button)
      app::states::system.current_screen =
          app::states::System::Screen::kDashboard;

    ImGui::SetCursorPosX(kStyleScreenPadding.x);
    const auto is_bookings_button =
        Button("Bookings", ImVec2(button_width, 0.f), false,
               current_screen == app::states::System::Screen::kBookings
                   ? widgets::ColorAccent::kPrimaryLight
                   : widgets::ColorAccent::kPrimaryBlank);

    if (is_bookings_button)
      app::states::system.current_screen =
          app::states::System::Screen::kBookings;

    ImGui::SetCursorPosX(kStyleScreenPadding.x);
    const auto is_rooms_button =
        Button("Rooms", ImVec2(button_width, 0.f), false,
               current_screen == app::states::System::Screen::kRooms
                   ? widgets::ColorAccent::kPrimaryLight
                   : widgets::ColorAccent::kPrimaryBlank);

    if (is_rooms_button)
      app::states::system.current_screen = app::states::System::Screen::kRooms;

    ImGui::SetCursorPosX(kStyleScreenPadding.x);
    const auto is_guests_button =
        Button("Guests", ImVec2(button_width, 0.f), false,
               current_screen == app::states::System::Screen::kGuests
                   ? widgets::ColorAccent::kPrimaryLight
                   : widgets::ColorAccent::kPrimaryBlank);

    if (is_guests_button)
      app::states::system.current_screen = app::states::System::Screen::kGuests;
  }

  ImGui::EndChild();
  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}

namespace layouts {
bool BeginAppLayout(const char *screen_heading) {
  ImGui::SetNextWindowPos(ImVec2());
  ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
  ImGui::Begin("app_layout", nullptr, kWindowDefaultFlags);

  const auto opened_hotel_id = app::states::system.opened_hotel_id;
  const auto viewport_size = ImGui::GetMainViewport()->WorkSize;
  const float nav_bar_width = opened_hotel_id == std::nullopt ? 0.f : 192.f;
  const float app_bar_height = 56.f;

  AppBar(screen_heading, app_bar_height, nav_bar_width);

  if (app::states::system.opened_hotel_id != std::nullopt)
    NavBar(nav_bar_width);

  // Screen Content
  {
    ImGui::SetNextWindowPos(ImVec2(nav_bar_width, app_bar_height),
                            ImGuiCond_Always, ImVec2());
    ImGui::SetNextWindowSize(ImVec2(viewport_size.x - nav_bar_width,
                                    viewport_size.y - app_bar_height));

    ImGui::BeginChild("app_layout_screen");
  }

  const auto last_screen_heading_temp = last_screen_heading;
  last_screen_heading = std::string(screen_heading);

  return last_screen_heading != last_screen_heading_temp;
}

bool EndAppLayout() {
  ImGui::EndChild();
  ImGui::End();

  const auto is_prepare_unmount_temp = is_prepare_unmount;
  is_prepare_unmount = false;

  return is_prepare_unmount_temp;
}
}  // namespace layouts
