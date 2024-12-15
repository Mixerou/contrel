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

// Texts
void BodyText(const char *fmt, ...);

void HeadingXlText(const char *fmt, ...);

void HeadingXlTextCenter(const char *fmt, ...);

// Application Status Badges
void ErrorAppBadge(const char *fmt,
                   ColorAccent color_accent = ColorAccent::kPrimary);
}  // namespace widgets

#endif  // DESKTOP_CLIENT_WIDGETS_H
