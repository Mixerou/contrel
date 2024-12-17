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
  static char email[128] = "";
  static char password[128] = "";
  static bool is_requesting = false;
  static std::string error = "";
  static backend::BackendRequest request;
  auto style = ImGui::GetStyle();

  layouts::BeginAuthLayout();

  // Heading
  {
    widgets::HeadingXlTextCenter("Welcome Back");
  }

  // Error Text
  {
    ImGui::PushStyleColor(ImGuiCol_Text, kColorDanger500);
    if (error != "") widgets::BodyTextCenter(error.c_str());
    ImGui::PopStyleColor();
  }

  // Inputs
  {
    widgets::MetaInputText("Email", email, IM_ARRAYSIZE(email),
                           ImGuiInputTextFlags_CharsNoBlank);
    widgets::MetaInputText("Password", password, IM_ARRAYSIZE(password),
                           ImGuiInputTextFlags_Password);
  }

  ImGui::Spacing();

  // Button
  {
    float button_width =
        ImGui::GetContentRegionAvail().x / 2.0 - style.ItemSpacing.x / 2.0;

    bool button_clicked =
        widgets::Button("Login", ImVec2(button_width, 0.0), is_requesting);

    if (button_clicked) {
      is_requesting = true;
      request = backend::Login({email, password});
    }
  }

  if (is_requesting) {
    backend::EmptyResponse empty_response;
    auto response = backend::GetResponse(request, empty_response);

    if (response == backend::ResponseStatus::kCompleted) {
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
