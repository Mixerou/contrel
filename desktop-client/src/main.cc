//
// Created by Mixerou on 14.12.2024.
//

#include <glad/gl.h>
#include <glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <ixwebsocket/IXNetSystem.h>
#include <utils.h>

#include "app.h"
#include "backend.h"
#include "constants.h"
#include "screens.h"
#include "widgets.h"

using namespace constants;

static void GlfwErrorCallback(const int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char **) {
  ix::initNetSystem();

  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit()) return EXIT_FAILURE;

  const auto glsl_version = "#version 150";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(1280, 680, "Contrel", nullptr, nullptr);
  if (window == nullptr) return EXIT_FAILURE;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (gladLoadGL(glfwGetProcAddress) == 0) return EXIT_FAILURE;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext(nullptr);

  ImGuiIO &io = ImGui::GetIO();
  io.IniFilename = nullptr;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  app::InitFonts();
  app::InitStyle();

  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

#ifdef DEBUG_BUILD
    {
      static bool is_debug_session = false;
      static bool show_demo_window = false;

      if (ImGui::IsKeyChordPressed(ImGuiMod_Ctrl | ImGuiKey_F11))
        is_debug_session = !is_debug_session;

      if (is_debug_session) {
        ImGui::Begin("Debug", nullptr, ImGuiWindowFlags_NoResize);

        ImGui::Text("AVG:  %.3f ms/frame | FPS: %.1f", 1000.f / io.Framerate,
                    io.Framerate);
        ImGui::Checkbox("Show demo window", &show_demo_window);

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::End();
      }
    }
#endif

    {  // Dumb ping loop
      static int64_t last_ping_response_at;
      static bool is_ping_requested = false;
      static backend::BackendRequest request;

      if (is_ping_requested) {
        backend::ping_response_t response_body;
        const auto response = GetResponse(request, response_body);

        if (response != backend::ResponseStatus::kInProcess) {
          last_ping_response_at = utils::GetCurrentUnixTimestamp();
          is_ping_requested = false;
        }

        if (response == backend::ResponseStatus::kCompleted)
          app::states::system.is_online = true;
        else if (response != backend::ResponseStatus::kInProcess)
          app::states::system.is_online = false;
      } else if (utils::GetCurrentUnixTimestamp() - last_ping_response_at >=
                 5) {
        is_ping_requested = true;
        request = backend::Ping();
      }

      if (!app::states::system.is_online)
        ErrorAppBadge("We're currently offline", widgets::ColorAccent::kDanger);
    }

    switch (app::states::system.current_screen) {
      case app::states::System::Screen::kAuth:
        screens::LoginScreen();
        break;
      case app::states::System::Screen::kDashboard:
        screens::DashboardScreen();
        break;
      case app::states::System::Screen::kBookings:
        screens::BookingsScreen();
        break;
      case app::states::System::Screen::kGuests:
        screens::GuestsScreen();
        break;
      case app::states::System::Screen::kHotels:
        screens::HotelsScreen();
        break;
      case app::states::System::Screen::kRooms:
        screens::RoomsScreen();
        break;
      default:
        break;
    }

    ImGui::Render();
    int display_width, display_height;
    glfwGetFramebufferSize(window, &display_width, &display_height);
    glViewport(0, 0, display_width, display_height);

    const auto clear_color = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  ix::uninitNetSystem();

  return EXIT_SUCCESS;
}
