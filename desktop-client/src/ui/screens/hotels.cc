//
// Created by Mixerou on 17.12.2024.
//

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

void HotelsTable() {
  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 658.0;

  ImGui::SetCursorPosX(available_region.x / 2.0 - hardcoded_table_width / 2.0);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("hotels", 4, kDefaultTableFlags)) {
    const auto column_flags = kDefaultTableColumnFlags;

    ImGui::TableSetupColumn("", column_flags, 256.0);
    ImGui::TableSetupColumn("", column_flags, 128.0);
    ImGui::TableSetupColumn("", column_flags, 128.0);
    ImGui::TableSetupColumn("", column_flags, 146.0);

    ImGui::TableHeadersRow();

    ImGui::TableSetColumnIndex(0);
    top_left_header_point = widgets::TableHeaderCellText("Hotel").top_left;

    ImGui::TableSetColumnIndex(1);
    widgets::TableHeaderCellText("Owner");

    ImGui::TableSetColumnIndex(2);
    widgets::TableHeaderCellText("Since");

    ImGui::TableSetColumnIndex(3);
    bottom_right_header_point = widgets::TableHeaderCellText("").bottom_right;

    int index = 0;
    for (const auto& hotel : app::states::data.hotels) {
      ImGui::PushID(std::format("hotel{}", hotel.first).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto first_cell_points =
          widgets::TableCellText(hotel.second.name.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      const auto owner = app::states::data.users[hotel.second.owner_id];
      widgets::TableCellText(
          std::format("{} {}", owner.first_name, owner.last_name).c_str(),
          true);

      ImGui::TableSetColumnIndex(2);
      widgets::TableCellText(
          utils::FormatUnixTimestampToHumanReadable(hotel.second.created_at)
              .c_str(),
          true);

      ImGui::TableSetColumnIndex(3);
      const auto button_text = "Let me in";
      const auto button_height =
          widgets::CalculateBodyText(button_text).y + kStyleButtonPadding.y * 2;
      const auto second_cell_points =
          widgets::BeginTableBodyCell(button_text, button_height);
      const auto is_open_button = widgets::Button(button_text);
      widgets::EndTableBodyCell();

      if (is_open_button) {
        app::states::system.current_screen =
            app::states::System::Screen::kDashboard;
        app::states::system.opened_hotel_id = hotel.first;
      }

      if (index == app::states::data.hotels.size() - 1)
        bottom_right_body_point = second_cell_points.bottom_right;

      ImGui::PopID();

      ++index;
    }

    ImGui::EndTable();
  }

  widgets::DrawTableHeaderBackground(widgets::TableCellScreenPosition(
      top_left_header_point, bottom_right_header_point));
  widgets::DrawTableBodyBackground(widgets::TableCellScreenPosition(
      top_left_body_point, bottom_right_body_point));
}

namespace screens {
void HotelsScreen() {
  static bool is_requesting = false;
  static bool is_request_made = false;
  static backend::BackendRequest request;

  if (layouts::BeginAppLayout("Your Hotels")) is_request_made = false;

  if (!is_request_made) {
    is_requesting = true;
    is_request_made = true;
    request = backend::GetAllHotels();
  }

  if (is_requesting) {
    backend::get_all_hotels_response_t get_all_hotels_response;
    auto response = backend::GetResponse(request, get_all_hotels_response);

    if (response == backend::ResponseStatus::kCompleted) {
      for (const auto hotel : get_all_hotels_response) {
        app::states::data.hotels[hotel.id] = hotel;
      }
    }

    if (response != backend::ResponseStatus::kInProcess) is_requesting = false;
  }

  auto available_region = ImGui::GetContentRegionAvail();

  // Top Bar
  {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0, 1.0, 1.0, 1.0));
    ImGui::SetNextWindowSize(ImVec2(available_region.x, 44.0));
    ImGui::BeginChild("top_bar");

    auto available_region = ImGui::GetContentRegionAvail();
    auto text = app::states::data.hotels.empty()
                    ? "Create you first hotel | Not available yet"
                    : "Select hotel or create a new one | WIP";
    auto text_size = widgets::CalculateBodyText(text);
    auto text_position = ImVec2(kStyleScreenPadding.x,
                                available_region.y / 2.0 - text_size.y / 2.0);

    ImGui::SetCursorPos(text_position);
    widgets::BodyTextDimmed(text);

    ImGui::EndChild();
    ImGui::PopStyleColor();
  }

  if (!app::states::data.hotels.empty()) {
    ImGui::Spacing();
    HotelsTable();
    ImGui::Spacing();
  }

  if (layouts::EndAppLayout()) {
    is_requesting = false;
    is_request_made = false;
  }
}
}  // namespace screens
