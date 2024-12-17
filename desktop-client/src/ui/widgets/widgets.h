//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_WIDGETS_H
#define DESKTOP_CLIENT_WIDGETS_H

#include <imgui.h>

namespace widgets {
enum class Alignment {
  kLeft,  // Default
  kCenter,
  kRight,
};

enum class ColorAccent {
  kPrimary,
  kDanger,
};

// Buttons
bool Button(const char *label, const ImVec2 &size = ImVec2(0, 0),
            const bool is_disabled = false,
            const ColorAccent color_accent = ColorAccent::kPrimary);

// Inputs
bool InputText(const char *label, char *buf, size_t buf_size,
               ImGuiInputTextFlags flags = 0,
               ImGuiInputTextCallback callback = NULL, void *user_data = NULL);

bool MetaInputText(const char *label, char *buf, size_t buf_size,
                   ImGuiInputTextFlags flags = 0,
                   ImGuiInputTextCallback callback = NULL,
                   void *user_data = NULL);

// Layout
void SameLine(const float spacing = 0.0);

// Texts
void BodyText(const char *fmt, ...);

void BodyTextCenter(const char *fmt, ...);

void HeadingXlText(const char *fmt, ...);

void HeadingXlTextCenter(const char *fmt, ...);

// Application Status Badges
void ErrorAppBadge(const char *fmt,
                   const ColorAccent color_accent = ColorAccent::kPrimary);
}  // namespace widgets

#endif  // DESKTOP_CLIENT_WIDGETS_H
