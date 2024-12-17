//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "app.h"
#include "backend.h"
#include "constants.h"
#include "layouts.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

namespace screens {
void LoginScreen() {
  // I know these buffers are larger that the backend accepts
  static char first_name[128] = "";
  static char last_name[128] = "";
  static char email[128] = "";
  static char password[128] = "";
  static char password_confirmation[128] = "";
  static bool is_registration = false;
  static bool is_requesting = false;
  static std::string error = "";
  static backend::BackendRequest request;
  auto style = ImGui::GetStyle();

  layouts::BeginAuthLayout();

  // Heading
  {
    const char *heading = is_registration ? "Welcome" : "Welcome Back";

    widgets::HeadingXlTextCenter("%s", heading);
  }

  // Error Text
  {
    ImGui::PushStyleColor(ImGuiCol_Text, kColorDanger500);
    if (error != "") widgets::BodyTextCenter(error.c_str());
    ImGui::PopStyleColor();
  }

  // Inputs
  {
    if (is_registration) {
      const float input_group_width =
          ImGui::GetContentRegionAvail().x / 2.0 - style.ItemSpacing.x / 2.0;
      const ImVec2 cursor_screen_position = ImGui::GetCursorScreenPos();

      ImGui::PushItemWidth(input_group_width);

      ImGui::BeginGroup();
      widgets::MetaInputText("First name", first_name,
                             IM_ARRAYSIZE(first_name));
      ImGui::EndGroup();

      ImGui::SetCursorScreenPos(ImVec2(
          cursor_screen_position.x + input_group_width + style.ItemSpacing.x,
          cursor_screen_position.y));
      ImGui::BeginGroup();
      widgets::MetaInputText("Last name", last_name, IM_ARRAYSIZE(last_name));
      ImGui::EndGroup();

      ImGui::PopItemWidth();
    }

    widgets::MetaInputText("Email", email, IM_ARRAYSIZE(email),
                           ImGuiInputTextFlags_CharsNoBlank);
    widgets::MetaInputText("Password", password, IM_ARRAYSIZE(password),
                           ImGuiInputTextFlags_Password);

    if (is_registration)
      widgets::MetaInputText("Confirm password", password_confirmation,
                             IM_ARRAYSIZE(password_confirmation),
                             ImGuiInputTextFlags_Password);
  }

  ImGui::Spacing();

  // Buttons
  {
    float button_width =
        ImGui::GetContentRegionAvail().x / 2.0 - style.ItemSpacing.x / 2.0;
    const char *left_button_text = is_registration ? "Back" : "Register";
    const char *right_button_text = is_registration ? "Continue" : "Login";

    bool left_button_clicked = widgets::Button(
        left_button_text, ImVec2(button_width, 0.0), is_requesting);
    widgets::SameLine();
    bool right_button_clicked = widgets::Button(
        right_button_text, ImVec2(button_width, 0.0), is_requesting);

    if (left_button_clicked) {
      is_registration = !is_registration;
      error = "";
    }

    if (left_button_clicked && is_registration) password_confirmation[0] = '\0';

    if (right_button_clicked && is_registration) {
      if (std::strcmp(password, password_confirmation) == 0) {
        is_requesting = true;
        request = backend::Register({email, password, first_name, last_name});
      } else
        error = "Passwords are not identical";
    }

    if (right_button_clicked && !is_registration) {
      is_requesting = true;
      request = backend::Login({email, password});
    }
  }

  if (is_requesting) {
    backend::EmptyResponse empty_response;
    auto response = backend::GetResponse(request, empty_response);

    if (response == backend::ResponseStatus::kCompleted) {
      first_name[0] = '\0';
      last_name[0] = '\0';
      email[0] = '\0';
      password[0] = '\0';
      password_confirmation[0] = '\0';
      is_registration = false;
      is_requesting = false;
      error = "";
      app::states::system.current_screen = app::states::System::Screen::kHotels;
    } else if (response == backend::ResponseStatus::kCompetedWithError) {
      is_requesting = false;
      error = request.error_response.message;
    } else if (response == backend::ResponseStatus::kError) {
      is_requesting = false;
      error = "Something went wrong";
    }
  }

  layouts::EndAuthLayout();
}
}  // namespace screens
