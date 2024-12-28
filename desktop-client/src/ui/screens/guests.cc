//
// Created by Mixerou on 19.12.2024.
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

enum class GuestsScreenView {
  kGuestsTable,
  kGuestCreation,
  kGuestView,
};

enum class GuestsScreenRequestType {
  kGetAllGuests,
  kCreateGuest,
  kNone,
};

struct NewGuest {
  char first_name[128];
  char last_name[128];
  char date_of_birth[11];
  entities::Gender gender;
  char phone_number[32];
  char email[128];
  entities::DocumentType document_type;
  char document_number[128];
  entities::Country document_country;
  char document_valid_until[11];
  char notes[128];

  NewGuest()
      : first_name(""),
        last_name(""),
        date_of_birth(""),
        gender(entities::Gender::kMale),
        phone_number(""),
        email(""),
        document_type(entities::DocumentType::kPassport),
        document_number(""),
        document_country(entities::Country::kIrelandRepublic),
        document_valid_until(""),
        notes("") {}

  backend::CreateGuestRequestPayload ToBackendCreateGuestRequestPayload() {
    return {std::string(first_name),
            std::string(last_name),
            utils::ConvertHumanReadableTimestampToUnix(date_of_birth),
            gender,
            std::string(phone_number),
            std::string(email),
            document_type,
            std::string(document_number),
            document_country,
            utils::ConvertHumanReadableTimestampToUnix(document_valid_until),
            std::string(notes)};
  }
};

struct GuestsScreenState {
  GuestsScreenView view = GuestsScreenView::kGuestsTable;
  entities::guest_id_t guest_to_view_id = 0;
  NewGuest new_guest = NewGuest();
  std::string creation_error;
  GuestsScreenRequestType request_type = GuestsScreenRequestType::kNone;
  bool is_initial_retrieved = false;
  backend::BackendRequest request;

  GuestsScreenState() = default;
};

static auto guests_screen_state = GuestsScreenState();

void GuestsTopBar() {
  ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.f, 1.f, 1.f, 1.f));
  ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 44.f));
  ImGui::BeginChild("top_bar");

  const auto available_region = ImGui::GetContentRegionAvail();
  std::string text;
  std::string button_text;

  if (guests_screen_state.view == GuestsScreenView::kGuestsTable &&
      app::states::data.guests.empty()) {
    text = "Add you first hotel guest";
    button_text = "Add guest";
  } else if (guests_screen_state.view == GuestsScreenView::kGuestsTable) {
    text = "Check existing guest or create a new one";
    button_text = "Add guest";
  } else if (guests_screen_state.view == GuestsScreenView::kGuestCreation) {
    text = "Add a new person";
    button_text = "Cancel";
  } else if (guests_screen_state.view == GuestsScreenView::kGuestView) {
    text = "Detailed person view";
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
  if (widgets::Button(
          button_text.c_str(), ImVec2(),
          guests_screen_state.request_type != GuestsScreenRequestType::kNone)) {
    guests_screen_state.new_guest = NewGuest();
    guests_screen_state.creation_error = "";

    if (guests_screen_state.view == GuestsScreenView::kGuestsTable)
      guests_screen_state.view = GuestsScreenView::kGuestCreation;
    else if (guests_screen_state.view == GuestsScreenView::kGuestCreation)
      guests_screen_state.view = GuestsScreenView::kGuestsTable;
    else if (guests_screen_state.view == GuestsScreenView::kGuestView) {
      guests_screen_state.view = GuestsScreenView::kGuestsTable;
      guests_screen_state.guest_to_view_id = 0.f;
    }
  }

  ImGui::EndChild();
  ImGui::PopStyleColor();
}

void GuestsTable() {
  if (!app::states::system.opened_hotel_id.has_value()) return;

  const auto hotel_id = app::states::system.opened_hotel_id.value();
  const auto available_region = ImGui::GetContentRegionAvail();
  const float hardcoded_table_width = 884.f;

  ImGui::SetCursorPosX(available_region.x / 2.f - hardcoded_table_width / 2.f);

  ImVec2 top_left_header_point;
  ImVec2 bottom_right_header_point;
  ImVec2 top_left_body_point;
  ImVec2 bottom_right_body_point;

  if (ImGui::BeginTable("guests", 6, kDefaultTableFlags)) {
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 128.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 224.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 160.f);
    ImGui::TableSetupColumn("", kDefaultTableColumnFlags, 116.f);

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
    widgets::TableHeaderCellText("Citizen of");

    ImGui::TableSetColumnIndex(5);
    bottom_right_header_point = widgets::TableHeaderCellText("").bottom_right;

    int index = 0;
    for (const auto& [guest_id, guest] : app::states::data.guests) {
      if (guest.hotel_id != hotel_id) continue;

      ImGui::PushID(std::format("guest{}", guest_id).c_str());
      ImGui::TableNextRow();

      ImGui::TableSetColumnIndex(0);
      const auto first_cell_points =
          widgets::TableCellText(guest.first_name.c_str());

      if (index == 0) top_left_body_point = first_cell_points.top_left;

      ImGui::TableSetColumnIndex(1);
      widgets::TableCellText(guest.last_name.c_str());

      ImGui::TableSetColumnIndex(2);
      widgets::TableCellText(guest.phone_number.c_str());

      ImGui::TableSetColumnIndex(3);
      widgets::TableCellText(guest.email.c_str());

      ImGui::TableSetColumnIndex(4);
      widgets::TableCellText(
          entities::kAllCountries[static_cast<int16_t>(guest.document_country)]
              .second);

      ImGui::TableSetColumnIndex(5);
      const auto button_text = "View";
      const auto button_height = widgets::CalculateBodyText(button_text).y +
                                 kStyleButtonPadding.y * 2.f;
      const auto second_cell_points =
          widgets::BeginTableBodyCell(button_text, button_height);
      const auto is_open_button = widgets::Button(button_text);
      widgets::EndTableBodyCell();

      if (is_open_button) {
        guests_screen_state.view = GuestsScreenView::kGuestView;
        guests_screen_state.guest_to_view_id = guest_id;
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

void GuestCreationView() {
  auto available_region = ImGui::GetContentRegionAvail();
  auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
  auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                              viewport_work_size.y - available_region.y);

  ImGui::SetNextWindowPos(
      ImVec2(viewport_work_size.x / 2.f + occupied_size.x / 2.f,
             viewport_work_size.y / 2.f + occupied_size.y / 2.f),
      ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::BeginChild("creation", ImVec2(), kChildWindowFitContent);

  // Inputs
  {
    const auto style = ImGui::GetStyle();
    const float items_width = 384.f;

    ImGui::PushItemWidth(items_width);
    ImGui::BeginGroup();

    // First name and last name in one row
    {
      const float items_width_half =
          items_width / 2.f - style.ItemSpacing.x / 2.f;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(items_width_half);

      ImGui::BeginGroup();
      widgets::MetaInputText(
          "First name", guests_screen_state.new_guest.first_name,
          IM_ARRAYSIZE(guests_screen_state.new_guest.first_name));
      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();
      widgets::MetaInputText(
          "Last name", guests_screen_state.new_guest.last_name,
          IM_ARRAYSIZE(guests_screen_state.new_guest.last_name));
      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    // Date of birth and gender in one row
    {
      const float items_width_half =
          items_width / 2.f - style.ItemSpacing.x / 2.f;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(items_width_half);

      ImGui::BeginGroup();
      widgets::MetaInputText(
          "Date of birth", guests_screen_state.new_guest.date_of_birth,
          IM_ARRAYSIZE(guests_screen_state.new_guest.date_of_birth),
          ImGuiInputTextFlags_CallbackAlways, widgets::FilterInputDate);
      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();

      // Gender
      {
        const auto preview_gender =
            entities::kAllGenders[static_cast<int>(
                                      guests_screen_state.new_guest.gender)]
                .second;
        const auto gender_combo = widgets::BeginCombo("Gender", preview_gender);

        if (gender_combo) {
          for (const auto& [gender_type, gender_name] : entities::kAllGenders) {
            const bool is_selected =
                (guests_screen_state.new_guest.gender == gender_type);

            if (ImGui::Selectable(gender_name, is_selected))
              guests_screen_state.new_guest.gender = gender_type;
            if (is_selected) ImGui::SetItemDefaultFocus();
          }
        }

        widgets::EndCombo(gender_combo);
      }

      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    // Contact info in one row
    {
      const float items_width_half =
          items_width / 2.F - style.ItemSpacing.x / 2.f;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(items_width_half);

      ImGui::BeginGroup();
      widgets::MetaInputText(
          "Phone number", guests_screen_state.new_guest.phone_number,
          IM_ARRAYSIZE(guests_screen_state.new_guest.phone_number),
          ImGuiInputTextFlags_CallbackAlways, widgets::FilterInputPhoneNumber);
      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();
      widgets::MetaInputText("Email", guests_screen_state.new_guest.email,
                             IM_ARRAYSIZE(guests_screen_state.new_guest.email),
                             ImGuiInputTextFlags_CharsNoBlank);
      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    // Main document meta in one row
    {
      const float items_width_half =
          items_width / 2.f - style.ItemSpacing.x / 2.f;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(items_width_half);

      ImGui::BeginGroup();

      // Document Type
      {
        const auto preview_document_type =
            entities::kAllDocumentTypes
                [static_cast<int>(guests_screen_state.new_guest.document_type)]
                    .second;
        const auto document_type_combo =
            widgets::BeginCombo("Document type", preview_document_type);

        if (document_type_combo) {
          for (const auto& [document_type, document_name] :
               entities::kAllDocumentTypes) {
            const bool is_selected =
                (guests_screen_state.new_guest.document_type == document_type);

            if (ImGui::Selectable(document_name, is_selected))
              guests_screen_state.new_guest.document_type = document_type;
            if (is_selected) ImGui::SetItemDefaultFocus();
          }
        }

        widgets::EndCombo(document_type_combo);
      }

      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();

      // Document Country
      {
        const auto preview_document_country =
            entities::kAllCountries[static_cast<int>(
                                        guests_screen_state.new_guest
                                            .document_country)]
                .second;
        const auto document_country_combo =
            widgets::BeginCombo("Document country", preview_document_country,
                                ImGuiComboFlags_HeightLargest);

        if (document_country_combo) {
          for (const auto& [document_country_type, document_country_name] :
               entities::kAllCountries) {
            const bool is_selected =
                (guests_screen_state.new_guest.document_country ==
                 document_country_type);

            if (ImGui::Selectable(document_country_name, is_selected))
              guests_screen_state.new_guest.document_country =
                  document_country_type;
            if (is_selected) ImGui::SetItemDefaultFocus();
          }
        }

        widgets::EndCombo(document_country_combo);
      }

      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    // Date of birth and gender in one row
    {
      const float items_width_half =
          items_width / 2.f - style.ItemSpacing.x / 2.f;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(items_width_half);

      ImGui::BeginGroup();
      widgets::MetaInputText(
          "Document number", guests_screen_state.new_guest.document_number,
          IM_ARRAYSIZE(guests_screen_state.new_guest.document_number));
      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();
      widgets::MetaInputText(
          "Document valid until",
          guests_screen_state.new_guest.document_valid_until,
          IM_ARRAYSIZE(guests_screen_state.new_guest.document_valid_until),
          ImGuiInputTextFlags_CallbackAlways, widgets::FilterInputDate);
      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    widgets::MetaInputText("Notes", guests_screen_state.new_guest.notes,
                           IM_ARRAYSIZE(guests_screen_state.new_guest.notes));

    ImGui::EndGroup();
    ImGui::PopItemWidth();
  }

  // Error
  {
    ImGui::PushStyleColor(ImGuiCol_Text, kColorErrorText);
    if (!guests_screen_state.creation_error.empty())
      widgets::BodyTextCenter(guests_screen_state.creation_error.c_str());
    ImGui::PopStyleColor();
  }

  ImGui::Spacing();

  // Button
  {
    const auto is_create_button = widgets::Button(
        "Add", ImVec2(384.f, 0.f),
        guests_screen_state.request_type != GuestsScreenRequestType::kNone);

    if (is_create_button) {
      guests_screen_state.request_type = GuestsScreenRequestType::kCreateGuest;
      guests_screen_state.request = CreateGuest(
          app::states::system.opened_hotel_id.value(),
          guests_screen_state.new_guest.ToBackendCreateGuestRequestPayload());
    }
  }

  ImGui::EndChild();
}

void GuestView() {
  const auto available_region = ImGui::GetContentRegionAvail();
  const auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
  const auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                                    viewport_work_size.y - available_region.y);

  const auto guest =
      app::states::data.guests[guests_screen_state.guest_to_view_id];
  const auto gender =
      entities::kAllGenders[static_cast<int16_t>(guest.gender)].second;
  const auto date_of_birth =
      utils::FormatUnixTimestampToHumanReadable(guest.date_of_birth);
  const auto document_type =
      entities::kAllDocumentTypes[static_cast<int16_t>(guest.document_type)]
          .second;
  const auto document_country =
      entities::kAllCountries[static_cast<int16_t>(guest.document_country)]
          .second;
  const auto document_valid_until =
      utils::FormatUnixTimestampToHumanReadable(guest.document_valid_until);

  ImGui::SetNextWindowPos(
      ImVec2(viewport_work_size.x / 2.f + occupied_size.x / 2.f,
             viewport_work_size.y / 2.f + occupied_size.y / 2.f),
      ImGuiCond_Always, ImVec2(0.5f, 0.5f));
  ImGui::BeginChild("creation", ImVec2(), kChildWindowFitContent);

  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(16.f, 16.f));
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 16.f);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.f, 1.f, 1.f, 1.f));

  // Guest Information
  {
    ImGui::BeginChild("guest_information", ImVec2(),
                      ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeX |
                          ImGuiChildFlags_AutoResizeY);

    widgets::HeadingLargeText(
        std::format("{} {}", guest.first_name, guest.last_name).c_str());

    widgets::BodyText(
        std::format("Phone number: {}", guest.phone_number).c_str());
    widgets::BodyText(std::format("Email: {}", guest.email).c_str());
    widgets::BodyText(std::format("{} | {}", gender, date_of_birth).c_str());
    widgets::BodyText(std::format("From {}", document_country).c_str());
    widgets::BodyText(
        std::format("{} number: {}", document_type, guest.document_number)
            .c_str());
    widgets::BodyText(
        std::format("Document valid until {}", document_valid_until).c_str());

    if (guest.notes.empty()) widgets::BodyTextDimmed("There are no notes");

    ImGui::EndChild();
  }

  widgets::SameLine();

  // Notes
  if (!guest.notes.empty()) {
    ImGui::BeginChild("guest_notes", ImVec2(),
                      ImGuiChildFlags_FrameStyle | ImGuiChildFlags_AutoResizeX |
                          ImGuiChildFlags_AutoResizeY);

    widgets::HeadingLargeText("Notes");
    widgets::BodyText(guest.notes.c_str());

    ImGui::EndChild();
  }

  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);

  ImGui::EndChild();
}

namespace screens {
void GuestsScreen() {
  if (layouts::BeginAppLayout("Hotel Guests")) {
    guests_screen_state = GuestsScreenState();
  }

  if (!guests_screen_state.is_initial_retrieved) {
    guests_screen_state.request_type = GuestsScreenRequestType::kGetAllGuests;
    guests_screen_state.is_initial_retrieved = true;
    guests_screen_state.request =
        backend::GetAllGuests(app::states::system.opened_hotel_id.value());
  }

  if (guests_screen_state.request_type ==
      GuestsScreenRequestType::kGetAllGuests) {
    backend::get_all_guests_response_t get_all_guests_response;
    const auto response =
        GetResponse(guests_screen_state.request, get_all_guests_response);

    if (response != backend::ResponseStatus::kInProcess) {
      app::states::data.guests.clear();
      guests_screen_state.request_type = GuestsScreenRequestType::kNone;
    }

    if (response == backend::ResponseStatus::kCompleted)
      for (const auto& guest : get_all_guests_response)
        app::states::data.guests[guest.id] = guest.ToGuest();
  }

  else if (guests_screen_state.request_type ==
           GuestsScreenRequestType::kCreateGuest) {
    backend::create_guest_response_t create_guest_response;

    if (const auto response =
            GetResponse(guests_screen_state.request, create_guest_response);
        response == backend::ResponseStatus::kCompleted) {
      guests_screen_state.new_guest = NewGuest();
      guests_screen_state.creation_error = "";
      guests_screen_state.view = GuestsScreenView::kGuestsTable;

      app::states::data.guests[create_guest_response.id] =
          create_guest_response.ToGuest();

      guests_screen_state.request_type = GuestsScreenRequestType::kGetAllGuests;
      guests_screen_state.request =
          backend::GetAllGuests(app::states::system.opened_hotel_id.value());
    } else if (response == backend::ResponseStatus::kCompetedWithError) {
      guests_screen_state.creation_error =
          guests_screen_state.request.error_response.message;
      guests_screen_state.request_type = GuestsScreenRequestType::kNone;
    } else if (response == backend::ResponseStatus::kError) {
      guests_screen_state.creation_error = "Something went wrong";
      guests_screen_state.request_type = GuestsScreenRequestType::kNone;
    }
  }

  GuestsTopBar();

  if (guests_screen_state.view == GuestsScreenView::kGuestsTable &&
      !app::states::data.guests.empty()) {
    ImGui::Spacing();
    GuestsTable();
    ImGui::Spacing();
  } else if (guests_screen_state.view == GuestsScreenView::kGuestCreation) {
    GuestCreationView();
  } else if (guests_screen_state.view == GuestsScreenView::kGuestView) {
    GuestView();
  }

  if (layouts::EndAppLayout()) {
    guests_screen_state = GuestsScreenState();
  }
}
}  // namespace screens
