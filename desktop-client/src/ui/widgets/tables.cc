//
// Created by Mixerou on 18.12.2024.
//

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
TableCellScreenPosition BeginTableBodyCell(const char *id,
                                           const float widgets_height) {
  const auto available_region = ImGui::GetContentRegionAvail();

  ImGui::SetNextWindowSize(ImVec2(available_region.x, kTableBodyCellHeight));
  ImGui::BeginChild(id);

  const auto top_left_point = ImGui::GetCursorScreenPos();
  const auto bottom_right_point =
      ImVec2(top_left_point.x + available_region.x,
             top_left_point.y + kTableBodyCellHeight);

  ImGui::SetCursorPos(ImVec2(
      kTableCellPaddingX, kTableBodyCellHeight / 2.f - widgets_height / 2.f));

  return TableCellScreenPosition(top_left_point, bottom_right_point);
}

void EndTableBodyCell() { ImGui::EndChild(); }

TableCellScreenPosition BeginTableHeaderCell(const char *id,
                                             const float widgets_height) {
  const auto available_region = ImGui::GetContentRegionAvail();

  ImGui::SetNextWindowSize(ImVec2(available_region.x, kTableHeaderCellHeight));
  ImGui::BeginChild(id);

  const auto top_left_point = ImGui::GetCursorScreenPos();
  const auto bottom_right_point =
      ImVec2(top_left_point.x + available_region.x,
             top_left_point.y + kTableHeaderCellHeight);

  ImGui::SetCursorPos(ImVec2(
      kTableCellPaddingX, kTableHeaderCellHeight / 2.f - widgets_height / 2.f));

  return TableCellScreenPosition(top_left_point, bottom_right_point);
}

void EndTableHeaderCell() { ImGui::EndChild(); }

TableCellScreenPosition TableHeaderCellText(const char *text) {
  const auto text_size = CalculateBodyText(text);
  const auto screen_position = BeginTableHeaderCell(text, text_size.y);

  BodyTextDimmed(text);

  EndTableBodyCell();

  return screen_position;
}

TableCellScreenPosition TableCellText(const char *text, const bool is_dimmed) {
  const auto text_size = CalculateBodyText(text);
  const auto screen_position = BeginTableBodyCell(text, text_size.y);

  if (is_dimmed)
    BodyTextDimmed(text);
  else
    BodyText(text);

  EndTableBodyCell();

  return screen_position;
}

void DrawTableHeaderBackground(const TableCellScreenPosition position) {
  const auto draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(position.top_left, position.bottom_right,
                           ImGui::ColorConvertFloat4ToU32(kColorTableHeader),
                           kTableRounding, ImDrawFlags_RoundCornersTop);
}

void DrawTableBodyBackground(const TableCellScreenPosition position) {
  const auto draw_list = ImGui::GetWindowDrawList();
  draw_list->AddRectFilled(position.top_left, position.bottom_right,
                           ImGui::ColorConvertFloat4ToU32(kColorTableBody),
                           kTableRounding, ImDrawFlags_RoundCornersBottom);
}
}  // namespace widgets
