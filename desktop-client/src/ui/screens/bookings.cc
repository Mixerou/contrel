//
// Created by Mixerou on 27.12.2024.
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

enum class BookingsScreenView {
  kBookingsTable,
  kBookingGuestsTable,
  kBookingCreation,  // TODO: not implemented
};

enum class BookingsScreenRequestType {
  kGetAllBookings,
  kGetAllRooms,
  kGetAllGuests,
  kCreateBooking,  // TODO: not implemented
  kNone,
};

struct BookingsScreenState {
  BookingsScreenView view = BookingsScreenView::kBookingsTable;
  const entities::Booking* selected_booking = nullptr;
  std::vector<const entities::Guest*> selected_booking_guests;
  BookingsScreenRequestType request_type = BookingsScreenRequestType::kNone;
  bool is_initial_retrieved = false;
  backend::BackendRequest request;

  BookingsScreenState() = default;
};

static auto bookings_screen_state = BookingsScreenState();

void BookingsTopBar() {
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 1.f));
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 44.f));
  ImGui::BeginChild("top_bar");

  const auto available_region = ImGui::GetContentRegionAvail();
  std::string text;
  std::string button_text;

  if (bookings_screen_state.view == BookingsScreenView::kBookingsTable &&
      app::states::data.bookings.empty()) {
    text = "Cook you first booking";
    button_text = "Create booking";
  } else if (bookings_screen_state.view == BookingsScreenView::kBookingsTable) {
    text = "Check existing bookings or create a new one";
    button_text = "Create booking";
  } else if (bookings_screen_state.view ==
             BookingsScreenView::kBookingGuestsTable) {
    text = format(
        "The guests of room {} ({}) for the dates from {} to {}",
        app::states::data.rooms[bookings_screen_state.selected_booking->room_id]
            .number,
        app::states::data.rooms[bookings_screen_state.selected_booking->room_id]
            .group_name,
        utils::FormatUnixTimestampToHumanReadable(
            bookings_screen_state.selected_booking->check_in_at),
        utils::FormatUnixTimestampToHumanReadable(
            bookings_screen_state.selected_booking->check_out_at));
    button_text = "Back to all bookings";
  } else if (bookings_screen_state.view ==
             BookingsScreenView::kBookingCreation) {
    text = "Create a new booking";
    button_text = "Cancel";
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
                      bookings_screen_state.request_type !=
                          BookingsScreenRequestType::kNone)) {
    if (bookings_screen_state.view == BookingsScreenView::kBookingsTable)
      bookings_screen_state.view = BookingsScreenView::kBookingCreation;
    else
      bookings_screen_state.view = BookingsScreenView::kBookingsTable;
  }

  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void BookingsTable() {
  if (!app::states::system.opened_hotel_id.has_value()) return;

  const auto hotel_id = app::states::system.opened_hotel_id.value();
  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 674.f;

  ImGui::SetCursorPosX(available_region.x / 2.f - hardcoded_table_width / 2.f);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("bookings", 5, kDefaultTableFlags)) {
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 162.f);

    ImGui::TableHeadersRow();

    ImGui::TableSetColumnIndex(0);
    top_left_header_point = widgets::TableHeaderCellText("Room").top_left;

    ImGui::TableSetColumnIndex(1);
    widgets::TableHeaderCellText("Group");

    ImGui::TableSetColumnIndex(2);
    widgets::TableHeaderCellText("Check-in");

    ImGui::TableSetColumnIndex(3);
    widgets::TableHeaderCellText("Check-out");

    ImGui::TableSetColumnIndex(4);
    bottom_right_header_point = widgets::TableHeaderCellText("").bottom_right;

    int index = 0;
    for (const auto& [booking_id, booking] : app::states::data.bookings) {
      if (booking.hotel_id != hotel_id) continue;

      const auto* room = &app::states::data.rooms[booking.room_id];

      ImGui::PushID(std::format("booking{}", booking_id).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto first_cell_points =
          widgets::TableCellText(room->number.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      widgets::TableCellText(room->group_name.c_str());

      ImGui::TableSetColumnIndex(2);
      widgets::TableCellText(
          utils::FormatUnixTimestampToHumanReadable(booking.check_in_at)
              .c_str());

      ImGui::TableSetColumnIndex(3);
      widgets::TableCellText(
          utils::FormatUnixTimestampToHumanReadable(booking.check_out_at)
              .c_str());

      ImGui::TableSetColumnIndex(4);
      const auto button_text = "View guests";
      const auto button_height = widgets::CalculateBodyText(button_text).y +
                                 kStyleButtonPadding.y * 2.f;
      const auto second_cell_points =
          widgets::BeginTableBodyCell(button_text, button_height);
      const auto is_open_button = widgets::Button(button_text);
      widgets::EndTableBodyCell();

      if (is_open_button) {
        bookings_screen_state.selected_booking_guests.clear();
        bookings_screen_state.view = BookingsScreenView::kBookingGuestsTable;
        bookings_screen_state.selected_booking = &booking;

        for (const auto& [guest_id, guest] : app::states::data.guests) {
          if (std::ranges::find(booking.guest_ids, guest_id) ==
              booking.guest_ids.end())
            continue;

          bookings_screen_state.selected_booking_guests.push_back(&guest);
        }
      }

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

void BookingGuestsTable() {
  if (!app::states::system.opened_hotel_id.has_value()) return;

  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 768.f;

  ImGui::SetCursorPosX(available_region.x / 2.f - hardcoded_table_width / 2.f);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("booking_guests", 5, kDefaultTableFlags)) {
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 224.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 160.f);

    ImGui::TableHeadersRow();

    ImGui::TableSetColumnIndex(0);
    top_left_header_point = widgets::TableHeaderCellText("First name").top_left;

    ImGui::TableSetColumnIndex(1);
    widgets::TableHeaderCellText("Last name");

    ImGui::TableSetColumnIndex(2);
    widgets::TableHeaderCellText("Phone number");

    ImGui::TableSetColumnIndex(3);
    widgets::TableHeaderCellText("Email");

    ImGui::TableSetColumnIndex(4);
    bottom_right_header_point =
        widgets::TableHeaderCellText("Citizen of").bottom_right;

    int index = 0;
    for (const auto& guest : bookings_screen_state.selected_booking_guests) {
      ImGui::PushID(std::format("booking_guest{}", guest->id).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto first_cell_points =
          widgets::TableCellText(guest->first_name.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      widgets::TableCellText(guest->last_name.c_str());

      ImGui::TableSetColumnIndex(2);
      widgets::TableCellText(guest->phone_number.c_str());

      ImGui::TableSetColumnIndex(3);
      widgets::TableCellText(guest->email.c_str());

      ImGui::TableSetColumnIndex(4);
      const auto second_cell_points = widgets::TableCellText(
          entities::kAllCountries[static_cast<int16_t>(guest->document_country)]
              .second);

      if (index == bookings_screen_state.selected_booking_guests.size() - 1)
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

void BookingCreationView() {
  // TODO: implement
}

namespace screens {
void BookingsScreen() {
  if (layouts::BeginAppLayout("Hotel Bookings")) {
    bookings_screen_state = BookingsScreenState();
  }

  if (!bookings_screen_state.is_initial_retrieved) {
    bookings_screen_state.request_type =
        BookingsScreenRequestType::kGetAllBookings;
    bookings_screen_state.is_initial_retrieved = true;
    bookings_screen_state.request =
        backend::GetAllBookings(app::states::system.opened_hotel_id.value());
  }

  if (bookings_screen_state.request_type ==
      BookingsScreenRequestType::kGetAllBookings) {
    backend::get_all_bookings_response_t get_all_bookings_response;
    const auto response =
        GetResponse(bookings_screen_state.request, get_all_bookings_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.bookings.clear();
      bookings_screen_state.request_type =
          BookingsScreenRequestType::kGetAllRooms;
      bookings_screen_state.request =
          backend::GetAllRooms(app::states::system.opened_hotel_id.value());
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& booking : get_all_bookings_response)
        app::states::data.bookings[booking.id] = booking;
  }

  else if (bookings_screen_state.request_type ==
           BookingsScreenRequestType::kGetAllRooms) {
    backend::get_all_rooms_response_t get_all_rooms_response;
    const auto response =
        GetResponse(bookings_screen_state.request, get_all_rooms_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.rooms.clear();
      bookings_screen_state.request_type =
          BookingsScreenRequestType::kGetAllGuests;
      bookings_screen_state.request =
          backend::GetAllGuests(app::states::system.opened_hotel_id.value());
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& room : get_all_rooms_response)
        app::states::data.rooms[room.id] = room;
  }

  else if (bookings_screen_state.request_type ==
           BookingsScreenRequestType::kGetAllGuests) {
    backend::get_all_guests_response_t get_all_guests_response;
    const auto response =
        GetResponse(bookings_screen_state.request, get_all_guests_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.guests.clear();
      bookings_screen_state.request_type = BookingsScreenRequestType::kNone;
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& guest : get_all_guests_response)
        app::states::data.guests[guest.id] = guest.ToGuest();

  }

  else if (bookings_screen_state.request_type ==
           BookingsScreenRequestType::kCreateBooking) {
    // TODO: create booking
  }

  BookingsTopBar();

  if (bookings_screen_state.view == BookingsScreenView::kBookingsTable &&
      !app::states::data.bookings.empty()) {
    ImGui::Spacing();
    BookingsTable();
    ImGui::Spacing();
  } else if (bookings_screen_state.view ==
             BookingsScreenView::kBookingGuestsTable) {
    ImGui::Spacing();
    BookingGuestsTable();
    ImGui::Spacing();
  } else if (bookings_screen_state.view ==
             BookingsScreenView::kBookingCreation) {
    // TODO: BookingCreationView()
  }

  if (layouts::EndAppLayout()) {
    bookings_screen_state = BookingsScreenState();
  }
}
}  // namespace screens
