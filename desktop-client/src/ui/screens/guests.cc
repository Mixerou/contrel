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

namespace screens {
void GuestsScreen() {
  static NewGuest new_guest;
  static std::string creation_error;
  static bool is_creating_guest = false;
  static bool is_requesting = false;
  static bool is_request_made = false;
  static backend::BackendRequest request;

  if (layouts::BeginAppLayout("Hotel Guests")) {
    new_guest = NewGuest();
    creation_error = "";
    is_creating_guest = false;
    is_request_made = false;
  }

  if (!is_request_made) {
    is_requesting = true;
    is_request_made = true;
    request =
        backend::GetAllGuests(app::states::system.opened_hotel_id.value());
  }

  if (is_requesting && is_creating_guest) {
    backend::create_guest_response_t create_guest_response;
    auto response = backend::GetResponse(request, create_guest_response);

    if (response == backend::ResponseStatus::kCompleted) {
      new_guest = NewGuest();
      creation_error = "";
      is_creating_guest = false;

      app::states::data.guests[create_guest_response.id] =
          create_guest_response.ToGuest();

      request =
          backend::GetAllGuests(app::states::system.opened_hotel_id.value());
    } else if (response == backend::ResponseStatus::kCompetedWithError) {
      creation_error = request.error_response.message;
      is_requesting = false;
    } else if (response == backend::ResponseStatus::kError) {
      creation_error = "Something went wrong";
      is_requesting = false;
    }
  }

  if (is_requesting && !is_creating_guest) {
    backend::get_all_guests_response_t get_all_guests_response;
    auto response = backend::GetResponse(request, get_all_guests_response);

    if (response == backend::ResponseStatus::kCompleted) {
      app::states::data.guests.clear();

      for (const auto& guest : get_all_guests_response)
        app::states::data.guests[guest.id] = guest.ToGuest();
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
    const auto text = is_creating_guest ? "Add a new person"
                      : app::states::data.guests
                              .empty()  // Can have guests from another hotel
                          ? "Add you first hotel guest"
                          : "Check existing guest or create a new one";
    const auto text_size = widgets::CalculateBodyText(text);
    const auto text_position = ImVec2(
        kStyleScreenPadding.x, available_region.y / 2.0 - text_size.y / 2.0);

    ImGui::SetCursorPos(text_position);
    widgets::BodyTextDimmed(text);

    const auto button_text = is_creating_guest ? "Cancel" : "Add guest";
    const auto button_text_size = widgets::CalculateBodyText(button_text);
    const auto button_size =
        ImVec2(button_text_size.x + kStyleButtonPadding.x * 2,
               button_text_size.y + kStyleButtonPadding.y * 2);
    const auto button_position =
        ImVec2(available_region.x - kStyleScreenPadding.x - button_size.x,
               available_region.y / 2.0 - button_size.y / 2.0);

    ImGui::SetCursorPos(button_position);
    if (widgets::Button(button_text, ImVec2(0.0, 0.0), is_requesting)) {
      new_guest = NewGuest();
      creation_error = "";
      is_creating_guest = !is_creating_guest;
    }

    ImGui::EndChild();
    ImGui::PopStyleColor();
  }

  if (is_creating_guest) {
    auto available_region = ImGui::GetContentRegionAvail();
    auto viewport_work_size = ImGui::GetMainViewport()->WorkSize;
    auto occupied_size = ImVec2(viewport_work_size.x - available_region.x,
                                viewport_work_size.y - available_region.y);

    ImGui::SetNextWindowPos(
        ImVec2(viewport_work_size.x / 2.0 + occupied_size.x / 2.0,
               viewport_work_size.y / 2.0 + occupied_size.y / 2.0),
        ImGuiCond_Always, ImVec2(0.5, 0.5));
    ImGui::BeginChild("creation", ImVec2(0.0, 0.0), kChildWindowFitContent);

    // Inputs
    {
      const auto style = ImGui::GetStyle();
      const float items_width = 384.0;

      ImGui::PushItemWidth(items_width);
      ImGui::BeginGroup();

      // First name and last name in one row
      {
        const float items_width_half =
            items_width / 2.0 - style.ItemSpacing.x / 2.0;
        const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

        ImGui::PushItemWidth(items_width_half);

        ImGui::BeginGroup();
        widgets::MetaInputText("First name", new_guest.first_name,
                               IM_ARRAYSIZE(new_guest.first_name));
        ImGui::EndGroup();

        ImGui::SetCursorScreenPos(ImVec2(
            cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
            cursor_screen_position.y));
        ImGui::BeginGroup();
        widgets::MetaInputText("Last name", new_guest.last_name,
                               IM_ARRAYSIZE(new_guest.last_name));
        ImGui::EndGroup();

        ImGui::PopItemWidth();
      }

      // Date of birth and gender in one row
      {
        const float items_width_half =
            items_width / 2.0 - style.ItemSpacing.x / 2.0;
        const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

        ImGui::PushItemWidth(items_width_half);

        ImGui::BeginGroup();
        widgets::MetaInputText("Date of birth", new_guest.date_of_birth,
                               IM_ARRAYSIZE(new_guest.date_of_birth),
                               ImGuiInputTextFlags_CallbackAlways,
                               widgets::FilterInputDate);
        ImGui::EndGroup();

        ImGui::SetCursorScreenPos(ImVec2(
            cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
            cursor_screen_position.y));
        ImGui::BeginGroup();

        // Gender
        {
          const auto preview_gender =
              entities::kAllGenders[static_cast<int>(new_guest.gender)].second;
          const auto gender_combo =
              widgets::BeginCombo("Gender", preview_gender);

          if (gender_combo) {
            for (const auto& [gender_type, gender_name] :
                 entities::kAllGenders) {
              const bool is_selected = (new_guest.gender == gender_type);

              if (ImGui::Selectable(gender_name, is_selected))
                new_guest.gender = gender_type;
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
            items_width / 2.0 - style.ItemSpacing.x / 2.0;
        const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

        ImGui::PushItemWidth(items_width_half);

        ImGui::BeginGroup();
        widgets::MetaInputText("Phone number", new_guest.phone_number,
                               IM_ARRAYSIZE(new_guest.phone_number),
                               ImGuiInputTextFlags_CallbackAlways,
                               widgets::FilterInputPhoneNumber);
        ImGui::EndGroup();

        ImGui::SetCursorScreenPos(ImVec2(
            cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
            cursor_screen_position.y));
        ImGui::BeginGroup();
        widgets::MetaInputText("Email", new_guest.email,
                               IM_ARRAYSIZE(new_guest.email),
                               ImGuiInputTextFlags_CharsNoBlank);
        ImGui::EndGroup();

        ImGui::PopItemWidth();
      }

      // Main document meta in one row
      {
        const float items_width_half =
            items_width / 2.0 - style.ItemSpacing.x / 2.0;
        const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

        ImGui::PushItemWidth(items_width_half);

        ImGui::BeginGroup();

        // Document Type
        {
          const auto preview_document_type =
              entities::kAllDocumentTypes[static_cast<int>(
                                              new_guest.document_type)]
                  .second;
          const auto document_type_combo =
              widgets::BeginCombo("Document type", preview_document_type);

          if (document_type_combo) {
            for (const auto& [document_type, document_name] :
                 entities::kAllDocumentTypes) {
              const bool is_selected =
                  (new_guest.document_type == document_type);

              if (ImGui::Selectable(document_name, is_selected))
                new_guest.document_type = document_type;
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
                                          new_guest.document_country)]
                  .second;
          const auto document_country_combo =
              widgets::BeginCombo("Document country", preview_document_country,
                                  ImGuiComboFlags_HeightLargest);

          if (document_country_combo) {
            for (const auto& [document_country_type, document_country_name] :
                 entities::kAllCountries) {
              const bool is_selected =
                  (new_guest.document_country == document_country_type);

              if (ImGui::Selectable(document_country_name, is_selected))
                new_guest.document_country = document_country_type;
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
            items_width / 2.0 - style.ItemSpacing.x / 2.0;
        const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

        ImGui::PushItemWidth(items_width_half);

        ImGui::BeginGroup();
        widgets::MetaInputText("Document number", new_guest.document_number,
                               IM_ARRAYSIZE(new_guest.document_number));
        ImGui::EndGroup();

        ImGui::SetCursorScreenPos(ImVec2(
            cursor_screen_position.x + items_width_half + style.ItemSpacing.x,
            cursor_screen_position.y));
        ImGui::BeginGroup();
        widgets::MetaInputText(
            "Document valid until", new_guest.document_valid_until,
            IM_ARRAYSIZE(new_guest.document_valid_until),
            ImGuiInputTextFlags_CallbackAlways, widgets::FilterInputDate);
        ImGui::EndGroup();

        ImGui::PopItemWidth();
      }

      widgets::MetaInputText("Notes", new_guest.notes,
                             IM_ARRAYSIZE(new_guest.notes));

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

    ImGui::Spacing();

    // Button
    {
      const auto is_create_button =
          widgets::Button("Add", ImVec2(384.0, 0.0), is_requesting);

      if (is_create_button) {
        is_requesting = true;
        request = backend::CreateGuest(
            app::states::system.opened_hotel_id.value(),
            new_guest.ToBackendCreateGuestRequestPayload());
      }
    }

    ImGui::EndChild();
  } else if (!app::states::data.hotels.empty()) {
    ImGui::Spacing();
    // TODO: GuestsTable();
    ImGui::Spacing();
  }

  if (layouts::EndAppLayout()) {
    new_guest = NewGuest();
    creation_error = "";
    is_creating_guest = false;
    is_requesting = false;
    is_request_made = false;
  }
}
}  // namespace screens
