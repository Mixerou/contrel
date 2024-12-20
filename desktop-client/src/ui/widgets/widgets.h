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
  kPrimaryLight,
  kPrimaryBlank,
  kDanger,
};

// Buttons
bool Button(const char *label, const ImVec2 &size = ImVec2(0, 0),
            const bool is_disabled = false,
            const ColorAccent color_accent = ColorAccent::kPrimary);

// Combos
bool BeginCombo(const char *label, const char *preview_value,
                ImGuiComboFlags flags = 0);

void EndCombo(bool begin_combo_state);

// Inputs
bool InputText(const char *label, char *buf, size_t buf_size,
               ImGuiInputTextFlags flags = 0,
               ImGuiInputTextCallback callback = NULL, void *user_data = NULL);

bool MetaInputText(const char *label, char *buf, size_t buf_size,
                   ImGuiInputTextFlags flags = 0,
                   ImGuiInputTextCallback callback = NULL,
                   void *user_data = NULL);

int FilterInputPhoneNumber(ImGuiInputTextCallbackData *data);

int FilterInputDate(ImGuiInputTextCallbackData *data);

// Layout
void SameLine(const float spacing = 0.0);

// Tables
struct TableCellScreenPosition {
  ImVec2 top_left;
  ImVec2 bottom_right;

  TableCellScreenPosition(ImVec2 top_left, ImVec2 bottom_right)
      : top_left(top_left), bottom_right(bottom_right) {}
};

TableCellScreenPosition BeginTableBodyCell(const char *id,
                                           const float widgets_height);

void EndTableCell();

TableCellScreenPosition BeginTableBodyCell(const char *id,
                                           const float widgets_height);

void EndTableBodyCell();

TableCellScreenPosition BeginTableHeaderCell(const char *id,
                                             const float widgets_height);

void EndTableHeaderCell();

TableCellScreenPosition TableHeaderCellText(const char *text);

TableCellScreenPosition TableCellText(const char *text,
                                      const bool is_dimmed = false);

void DrawTableHeaderBackground(const TableCellScreenPosition position);

void DrawTableBodyBackground(const TableCellScreenPosition position);

// Texts
ImVec2 CalculateBodyText(const char *text);

void BodyText(const char *fmt, ...);

void BodyTextDimmed(const char *fmt, ...);

void BodyTextCenter(const char *fmt, ...);

ImVec2 CalculateHeadingXlText(const char *text);

void HeadingXlText(const char *fmt, ...);

void HeadingXlTextCenter(const char *fmt, ...);

ImVec2 CalculateHeadingLargeText(const char *text);

void HeadingLargeText(const char *fmt, ...);

void HeadingLargeTextCenter(const char *fmt, ...);

// Application Status Badges
void ErrorAppBadge(const char *fmt,
                   const ColorAccent color_accent = ColorAccent::kPrimary);
}  // namespace widgets

#endif  // DESKTOP_CLIENT_WIDGETS_H
