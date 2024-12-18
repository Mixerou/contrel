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
  static char new_hotel_name[128] = "";
  static std::string creation_error = "";
  static bool is_creating_hotel = false;
  static bool is_requesting = false;
  static bool is_request_made = false;
  static backend::BackendRequest request;

  if (layouts::BeginAppLayout("Your Hotels")) {
    new_hotel_name[0] = '\0';
    creation_error = "";
    is_creating_hotel = false;
    is_request_made = false;
  }

  if (!is_request_made) {
    is_requesting = true;
    is_request_made = true;
    request = backend::GetAllHotels();
  }

  if (is_requesting && is_creating_hotel) {
    backend::create_hotel_response_t create_hotel_response;
    auto response = backend::GetResponse(request, create_hotel_response);

    if (response == backend::ResponseStatus::kCompleted) {
      new_hotel_name[0] = '\0';
      creation_error = "";
      is_creating_hotel = false;

      app::states::data.hotels[create_hotel_response.id] =
          create_hotel_response;

      request = backend::GetAllHotels();
    } else if (response == backend::ResponseStatus::kCompetedWithError) {
      creation_error = request.error_response.message;
      is_requesting = false;
    } else if (response == backend::ResponseStatus::kError) {
      creation_error = "Something went wrong";
      is_requesting = false;
    }
  }

  if (is_requesting && !is_creating_hotel) {
    backend::get_all_hotels_response_t get_all_hotels_response;
    auto response = backend::GetResponse(request, get_all_hotels_response);

    if (response == backend::ResponseStatus::kCompleted) {
      app::states::data.hotels.clear();

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

    const auto available_region = ImGui::GetContentRegionAvail();
    const auto text = is_creating_hotel ? "Create the hotel of your dreams"
                      : app::states::data.hotels.empty()
                          ? "Create you first hotel"
                          : "Select hotel or create a new one";
    const auto text_size = widgets::CalculateBodyText(text);
    const auto text_position = ImVec2(
        kStyleScreenPadding.x, available_region.y / 2.0 - text_size.y / 2.0);

    ImGui::SetCursorPos(text_position);
    widgets::BodyTextDimmed(text);

    const auto button_text =
        is_creating_hotel ? "Cancel creation" : "Create hotel";
    const auto button_text_size = widgets::CalculateBodyText(button_text);
    const auto button_size =
        ImVec2(button_text_size.x + kStyleButtonPadding.x * 2,
               button_text_size.y + kStyleButtonPadding.y * 2);
    const auto button_position =
        ImVec2(available_region.x - kStyleScreenPadding.x - button_size.x,
               available_region.y / 2.0 - button_size.y / 2.0);

    ImGui::SetCursorPos(button_position);
    if (widgets::Button(button_text, ImVec2(0.0, 0.0), is_requesting)) {
      new_hotel_name[0] = '\0';
      creation_error = "";
      is_creating_hotel = !is_creating_hotel;
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
  }

  if (is_creating_hotel) {
    auto available_region = ImGui::GetContentRegionAvail();
    auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
    auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                                viewport_work_size.y - available_region.y);

    ImGui::SetNextWindowPos(
        ImVec2(viewport_work_size.x / 2.0 + occupied_size.x / 2.0,
               viewport_work_size.y / 2.0 + occupied_size.y / 2.0),
        ImGuiCond_Always, ImVec2(0.5, 0.5));
    ImGui::BeginChild("creation", ImVec2(0.0, 0.0), kChildWindowFitContent);

    // Input
    {
      ImGui::PushItemWidth(384.0);
      ImGui::BeginGroup();
      widgets::MetaInputText("Hotel name", new_hotel_name,
                             IM_ARRAYSIZE(new_hotel_name));
      ImGui::EndGroup();
      ImGui::PopItemWidth();
    }

    // Error
    {
      ImGui::PushStyleColor(ImGuiCol_Text, kColorErrorText);
      if (!creation_error.empty())
        widgets::BodyTextCenter(creation_error.c_str());
      ImGui::PopStyleColor();
    }

    // Button
    {
      const auto is_create_button =
          widgets::Button("Create", ImVec2(384.0, 0.0), is_requesting);

      if (is_create_button) {
        is_requesting = true;
        request = backend::CreateHotel({new_hotel_name});
      }
    }

    ImGui::EndChild();
  } else if (!app::states::data.hotels.empty()) {
    ImGui::Spacing();
    HotelsTable();
    ImGui::Spacing();
  }

  if (layouts::EndAppLayout()) {
    new_hotel_name[0] = '\0';
    creation_error = "";
    is_creating_hotel = false;
    is_requesting = false;
    is_request_made = false;
  }
}
}  // namespace screens
