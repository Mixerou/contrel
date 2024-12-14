//
// Created by Mixerou on 14.12.2024.
//

#include <iostream>

#include <glad/gl.h>
#include <glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "app.h"
#include "constants.h"
#include "widgets.h"

using namespace constants;

static void GlfwErrorCallback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int, char **) {
  glfwSetErrorCallback(GlfwErrorCallback);
  if (!glfwInit()) return EXIT_FAILURE;

  const char *glsl_version = "#version 150";
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

        ImGui::Text("AVG:  %.3f ms/frame | FPS: %.1f", 1000.0 / io.Framerate,
                    io.Framerate);
        ImGui::Checkbox("Show demo window", &show_demo_window);

        if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::End();
      }
    }
#endif

    {
      auto center = ImGui::GetMainViewport()->GetWorkCenter();

      ImGui::SetNextWindowPos(ImVec2(center.x, center.y), ImGuiCond_Always,
                              ImVec2(0.5, 0.5));
      ImGui::Begin("home_screen", nullptr, kWindowDefaultFlags);

      widgets::HeadingXlTextCenter("Welcome to Contrel");

      ImGui::End();
    }

    ImGui::Render();
    int display_width, display_height;
    glfwGetFramebufferSize(window, &display_width, &display_height);
    glViewport(0, 0, display_width, display_height);

    ImVec4 clear_color = ImGui::GetStyleColorVec4(ImGuiCol_WindowBg);
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

  return EXIT_SUCCESS;
}
