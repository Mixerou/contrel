//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "app.h"
#include "constants.h"
#include "layouts.h"
#include "utils.h"
#include "widgets.h"

using namespace constants;

namespace layouts {
bool BeginAuthLayout() {
  auto is_web_socket_worker_started = app::web_socket_worker.IsStarted();
  auto is_web_socket_worker_ready =
      app::states::system.IsWebSocketWorkerReadyState();
  auto is_loading_template =
      is_web_socket_worker_started && !is_web_socket_worker_ready;

  if (is_web_socket_worker_started && is_web_socket_worker_ready)
    app::states::system.current_screen = app::states::System::Screen::kHotels;

  ImGui::SetNextWindowPos(ImVec2(0.0, 0.0));
  ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
  ImGui::Begin("auth_layout", nullptr, kWindowDefaultFlags);

  // Left side
  auto viewport_size = ImGui::GetMainViewport()->WorkSize;
  float viewport_width_half = viewport_size.x / 2.0;

  ImGui::PushStyleColor(ImGuiCol_ChildBg, kColorPrimary500);
  ImGui::SetNextWindowPos(ImVec2(0.0, 0.0), ImGuiCond_Always);
  ImGui::SetNextWindowSize(ImVec2(viewport_width_half, viewport_size.y),
                           ImGuiCond_Always);
  ImGui::BeginChild("auth_layout_left_side");

  // Leaves Image
  {
    ImGui::SetNextWindowPos(ImVec2(-8.0, -8.0), ImGuiCond_Always,
                            ImVec2(0.0, 0.0));
    ImGui::BeginChild("auth_layout_left_side_leaves_image", ImVec2(0.0, 0.0),
                      kChildWindowFitContent);

    const auto illustration_size = ImVec2(0.0, viewport_size.y / 2.5);
    static utils::Texture illustration("assets/images/leaves.svg",
                                       illustration_size);
    illustration.CheckSize(illustration_size);

    ImGui::Image(illustration.GetTexture(), illustration.GetSize());

    ImGui::EndChild();
  }

  // Room Image
  {
    ImGui::SetNextWindowPos(
        ImVec2(viewport_width_half - viewport_width_half / 2.0,
               viewport_size.y),
        ImGuiCond_Always, ImVec2(0.5, 1.0));
    ImGui::BeginChild("auth_layout_left_side_room_image", ImVec2(0.0, 0.0),
                      kChildWindowFitContent);

    const auto illustration_size = ImVec2(viewport_size.x / 2.5, 0.0);
    static utils::Texture illustration("assets/images/room.svg",
                                       illustration_size);
    illustration.CheckSize(illustration_size);

    ImGui::Image(illustration.GetTexture(), illustration.GetSize());

    ImGui::EndChild();
  }

  ImGui::EndChild();
  ImGui::PopStyleColor();

  // Right side with screen content
  {
    auto center = ImGui::GetMainViewport()->GetWorkCenter();

    ImGui::SetNextWindowPos(ImVec2(center.x + center.x / 2.0, center.y),
                            ImGuiCond_Always, ImVec2(0.5, 0.5));
    ImGui::BeginChild("auth_layout_right_side", ImVec2(0.0, 0.0),
                      kChildWindowFitContent);
  }

  if (is_loading_template) widgets::HeadingXlTextCenter("Welcome Back");

  return !is_loading_template;
}

void EndAuthLayout() {
  ImGui::EndChild();
  ImGui::End();
}
}  // namespace layouts
