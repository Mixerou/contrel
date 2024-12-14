//
// Created by Mixerou on 14.12.2024.
//

#include <imgui.h>
#include <imgui_internal.h>

#include "app.h"
#include "widgets.h"

void TextEx(const char *fmt, va_list args, widgets::Alignment alignment) {
  float offset_factor;

  switch (alignment) {
    case widgets::Alignment::kCenter:
      offset_factor = 0.5;
      break;
    case widgets::Alignment::kRight:
      offset_factor = 1.0;
      break;
    default:
      offset_factor = 0.0;
      break;
  }

  const char *text_start, *text_end;
  ImFormatStringToTempBufferV(&text_start, &text_end, fmt, args);

  float offset = (ImGui::GetContentRegionAvail().x -
                  ImGui::CalcTextSize(text_start, text_end).x) *
                 offset_factor;
  if (offset > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);

  ImGui::TextV(fmt, args);
}

void BodyTextEx(const char *fmt, va_list args, widgets::Alignment alignment) {
  TextEx(fmt, args, alignment);
}

void HeadingXlTextEx(const char *fmt, va_list args,
                     widgets::Alignment alignment) {
  ImGui::PushFont(app::heading_xl_font);
  TextEx(fmt, args, alignment);
  ImGui::PopFont();
}

namespace widgets {
void BodyText(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  BodyTextEx(fmt, args, widgets::Alignment::kLeft);
  va_end(args);
}

void HeadingXlText(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  HeadingXlTextEx(fmt, args, widgets::Alignment::kLeft);
  va_end(args);
}

void HeadingXlTextCenter(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  HeadingXlTextEx(fmt, args, widgets::Alignment::kCenter);
  va_end(args);
}
}  // namespace widgets