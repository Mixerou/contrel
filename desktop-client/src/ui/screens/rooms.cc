//
// Created by Mixerou on 24.12.2024.
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

enum class RoomsScreenView {
  kRoomsTable,
};

struct RoomsScreenState {
  RoomsScreenView view;
  bool is_requesting = false;
  bool is_rooms_retrieved = false;
  backend::BackendRequest request;

  RoomsScreenState() : view(RoomsScreenView::kRoomsTable) {}
};

static auto rooms_screen_state = RoomsScreenState();

void RoomsTopBar() {
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 1.f));
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 44.f));
  ImGui::BeginChild("top_bar");

  const auto available_region = ImGui::GetContentRegionAvail();
  std::string text;
  std::string button_text;

  if (rooms_screen_state.view == RoomsScreenView::kRoomsTable &&
      app::states::data.rooms.empty()) {
    text = "Create you first hotel room";
    button_text = "Create room | unimplemented";
  } else if (rooms_screen_state.view == RoomsScreenView::kRoomsTable) {
    text = "Check existing room or create a new one";
    button_text = "Create room | unimplemented";
  }

  const auto text_size = widgets::CalculateBodyText(text.c_str());
  const auto text_position = ImVec2(
      kStyleScreenPadding.x, available_region.y / 2.f - text_size.y / 2.f);

  const auto button_text_size = widgets::CalculateBodyText(button_text.c_str());
  const auto button_size =
      ImVec2(button_text_size.x + kStyleButtonPadding.x * 2.f,
             button_text_size.y + kStyleButtonPadding.y * 2.f);
  const auto button_position =
      ImVec2(available_region.x - kStyleScreenPadding.x - button_size.x,
             available_region.y / 2.f - button_size.y / 2.f);

  ImGui::SetCursorPos(text_position);
  widgets::BodyTextDimmed(text.c_str());

  ImGui::SetCursorPos(button_position);
  if (widgets::Button(button_text.c_str(), ImVec2(),
                      rooms_screen_state.is_requesting)) {
  }

  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void RoomsTable() {
  if (!app::states::system.opened_hotel_id.has_value()) return;

  const auto hotel_id = app::states::system.opened_hotel_id.value();
  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 384.f;

  ImGui::SetCursorPosX(available_region.x / 2.f - hardcoded_table_width / 2.f);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("rooms", 3, kDefaultTableFlags)) {
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);

    ImGui::TableHeadersRow();

    ImGui::TableSetColumnIndex(0);
    top_left_header_point = widgets::TableHeaderCellText("Room").top_left;

    ImGui::TableSetColumnIndex(1);
    widgets::TableHeaderCellText("Group");

    ImGui::TableSetColumnIndex(2);
    bottom_right_header_point =
        widgets::TableHeaderCellText("Creation date").bottom_right;

    int index = 0;
    for (const auto& [room_id, room] : app::states::data.rooms) {
      if (room.hotel_id != hotel_id) continue;

      ImGui::PushID(std::format("room{}", room_id).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto first_cell_points =
          widgets::TableCellText(room.number.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      widgets::TableCellText(room.group_name.c_str());

      ImGui::TableSetColumnIndex(2);
      // TODO: do not mutate every iteration
      bottom_right_body_point =
          widgets::TableCellText(
              utils::FormatUnixTimestampToHumanReadable(room.created_at)
                  .c_str())
              .bottom_right;

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
void RoomsScreen() {
  if (layouts::BeginAppLayout("Hotel Rooms")) {
    rooms_screen_state = RoomsScreenState();
  }

  if (!rooms_screen_state.is_rooms_retrieved) {
    rooms_screen_state.is_requesting = true;
    rooms_screen_state.is_rooms_retrieved = true;
    rooms_screen_state.request =
        backend::GetAllRooms(app::states::system.opened_hotel_id.value());
  }

  if (rooms_screen_state.is_requesting &&
      rooms_screen_state.view == RoomsScreenView::kRoomsTable) {
    backend::get_all_rooms_response_t get_all_rooms_response;
    const auto response =
        GetResponse(rooms_screen_state.request, get_all_rooms_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.rooms.clear();
      rooms_screen_state.is_requesting = false;
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& room : get_all_rooms_response)
        app::states::data.rooms[room.id] = room;
  }

  RoomsTopBar();

  if (rooms_screen_state.view == RoomsScreenView::kRoomsTable &&
      !app::states::data.rooms.empty()) {
    ImGui::Spacing();
    RoomsTable();
    ImGui::Spacing();
  }

  if (layouts::EndAppLayout()) {
    rooms_screen_state = RoomsScreenState();
  }
}
}  // namespace screens
