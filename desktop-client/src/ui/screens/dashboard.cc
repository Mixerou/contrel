//
// Created by Mixerou on 18.12.2024.
//

#include <algorithm>
#include <format>

#include <imgui.h>

#include "app.h"
#include "backend.h"
#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "utils.h"
#include "widgets.h"

using namespace constants;

enum class DashboardScreenRequestType {
  kGetStatsLongTimeGuests,
  kGetAllRooms,
  kGetAllGuests,
  kNone,
};

struct DashboardScreenState {
  DashboardScreenRequestType request_type = DashboardScreenRequestType::kNone;
  bool is_initial_retrieved = false;
  backend::BackendRequest request;

  DashboardScreenState() = default;
};

static auto dashboard_screen_state = DashboardScreenState();

void DashboardLongTimeGuestsTable() {
  if (!app::states::system.opened_hotel_id.has_value()) return;

  const auto hotel_id = app::states::system.opened_hotel_id.value();
  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 640.f;

  ImGui::SetCursorPosX(available_region.x / 2.f - hardcoded_table_width / 2.f);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("long_time_guests", 4, kDefaultTableFlags)) {
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 256.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);

    ImGui::TableHeadersRow();

    ImGui::TableSetColumnIndex(0);
    top_left_header_point = widgets::TableHeaderCellText("Guest").top_left;

    ImGui::TableSetColumnIndex(1);
    widgets::TableHeaderCellText("Room");

    ImGui::TableSetColumnIndex(2);
    widgets::TableHeaderCellText("Room group");

    ImGui::TableSetColumnIndex(3);
    bottom_right_header_point =
        widgets::TableHeaderCellText("Lives (in days)").bottom_right;

    int index = 0;
    for (const auto& long_time_guest : app::states::data.long_time_guests) {
      if (long_time_guest.hotel_id != hotel_id) continue;

      const auto* guest = &app::states::data.guests[long_time_guest.guest_id];
      const auto* room = &app::states::data.rooms[long_time_guest.room_id];

      ImGui::PushID(std::format("long_time_guest{}", index).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto guest_name =
          std::format("{} {}", guest->first_name, guest->last_name);
      const auto first_cell_points = widgets::TableCellText(guest_name.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      widgets::TableCellText(room->number.c_str());

      ImGui::TableSetColumnIndex(2);
      widgets::TableCellText(room->group_name.c_str());

      ImGui::TableSetColumnIndex(3);
      const int64_t live_time_difference =
          (long_time_guest.check_out_at - long_time_guest.check_in_at) / 86400;
      const auto second_cell_points =
          widgets::TableCellText(std::to_string(live_time_difference).c_str());

      // TODO: do not mutate every iteration
      bottom_right_body_point = second_cell_points.bottom_right;

      ImGui::PopID();

      ++index;
    }

    ImGui::EndTable();
  }

  DrawTableHeaderBackground(widgets::TableCellScreenPosition(
      top_left_header_point, bottom_right_header_point));
  DrawTableBodyBackground(widgets::TableCellScreenPosition(
      top_left_body_point, bottom_right_body_point));
}

namespace screens {
void DashboardScreen() {
  if (layouts::BeginAppLayout("Summary and Nerd Stats")) {
    dashboard_screen_state = DashboardScreenState();
  }

  if (!dashboard_screen_state.is_initial_retrieved) {
    dashboard_screen_state.request_type =
        DashboardScreenRequestType::kGetStatsLongTimeGuests;
    dashboard_screen_state.is_initial_retrieved = true;
    dashboard_screen_state.request = backend::GetStatsLongTimeGuests(
        app::states::system.opened_hotel_id.value());
  }

  if (dashboard_screen_state.request_type ==
      DashboardScreenRequestType::kGetStatsLongTimeGuests) {
    backend::get_stats_long_time_guests_response_t long_time_guests_response;
    const auto response =
        GetResponse(dashboard_screen_state.request, long_time_guests_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.long_time_guests.clear();
      dashboard_screen_state.request_type =
          DashboardScreenRequestType::kGetAllRooms;
      dashboard_screen_state.request =
          backend::GetAllRooms(app::states::system.opened_hotel_id.value());
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& guest : long_time_guests_response)
        app::states::data.long_time_guests.push_back(guest);
  }

  else if (dashboard_screen_state.request_type ==
           DashboardScreenRequestType::kGetAllRooms) {
    backend::get_all_rooms_response_t get_all_rooms_response;
    const auto response =
        GetResponse(dashboard_screen_state.request, get_all_rooms_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.rooms.clear();
      dashboard_screen_state.request_type =
          DashboardScreenRequestType::kGetAllGuests;
      dashboard_screen_state.request =
          backend::GetAllGuests(app::states::system.opened_hotel_id.value());
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& room : get_all_rooms_response)
        app::states::data.rooms[room.id] = room;
  }

  else if (dashboard_screen_state.request_type ==
           DashboardScreenRequestType::kGetAllGuests) {
    backend::get_all_guests_response_t get_all_guests_response;
    const auto response =
        GetResponse(dashboard_screen_state.request, get_all_guests_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.guests.clear();
      dashboard_screen_state.request_type = DashboardScreenRequestType::kNone;
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& guest : get_all_guests_response)
        app::states::data.guests[guest.id] = guest.ToGuest();
  }

  if (app::states::data.long_time_guests.empty()) {
    const auto available_region = ImGui::GetContentRegionAvail();
    const auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
    const auto occupied_size =
        ImVec2(viewport_work_size.x - available_region.x,
               viewport_work_size.y - available_region.y);

    ImGui::SetNextWindowPos(
        ImVec2(viewport_work_size.x / 2.f + occupied_size.x / 2.f,
               viewport_work_size.y / 2.f + occupied_size.y / 2.f),
        ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::BeginChild("info", ImVec2(), kChildWindowFitContent);
    widgets::HeadingXlTextCenter("There is nothing to check right now");
    ImGui::EndChild();
  } else {
    ImGui::Spacing();
    DashboardLongTimeGuestsTable();
    ImGui::Spacing();
  }

  if (layouts::EndAppLayout()) {
    dashboard_screen_state = DashboardScreenState();
  }
}
}  // namespace screens
