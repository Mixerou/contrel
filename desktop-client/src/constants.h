//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_CONSTANTS_H
#define DESKTOP_CLIENT_CONSTANTS_H

#include <string>

#include <imgui.h>

namespace constants {
// Scale
constexpr float kFontScaleFactor = 1.0;
constexpr float kFontSizeFactor = 1.0;
constexpr float kVectorTextureScaleFactor = 1.0;

// Scale (for the HiDPI monitors)
// constexpr float kFontScaleFactor = 0.5;
// constexpr float kFontSizeFactor = 2.0;
// constexpr float kVectorTextureScaleFactor = 2.0;

// Window Flags
constexpr ImGuiWindowFlags kWindowDefaultFlags =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
    ImGuiWindowFlags_NoFocusOnAppearing |
    ImGuiWindowFlags_NoBringToFrontOnFocus;
constexpr ImGuiWindowFlags kWindowFloatFlags =
    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
    ImGuiWindowFlags_NoSavedSettings;
constexpr ImGuiChildFlags kChildWindowFitContent =
    ImGuiChildFlags_AutoResizeX | ImGuiChildFlags_AutoResizeY |
    ImGuiChildFlags_AlwaysAutoResize;

// Table Flags
constexpr ImGuiTableFlags kDefaultTableFlags =
    ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_RowBg |
    ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingFixedFit |
    ImGuiTableFlags_NoHostExtendX;
constexpr ImGuiTableColumnFlags kDefaultTableColumnFlags =
    ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize |
    ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoHide |
    ImGuiTableColumnFlags_NoHeaderLabel;

// Colors
constexpr ImVec4 kColorPrimary50 = ImVec4(0.9, 0.94, 0.93, 1.0);
constexpr ImVec4 kColorPrimary100 = ImVec4(0.8, 0.88, 0.86, 1.0);
constexpr ImVec4 kColorPrimary200 = ImVec4(0.6, 0.77, 0.71, 1.0);
constexpr ImVec4 kColorPrimary300 = ImVec4(0.4, 0.65, 0.57, 1.0);
constexpr ImVec4 kColorPrimary400 = ImVec4(0.21, 0.54, 0.42, 1.0);
constexpr ImVec4 kColorPrimary500 = ImVec4(0.01, 0.42, 0.28, 1.0);
constexpr ImVec4 kColorPrimary600 = ImVec4(0.01, 0.34, 0.22, 1.0);
constexpr ImVec4 kColorPrimary700 = ImVec4(0.0, 0.25, 0.17, 1.0);
constexpr ImVec4 kColorPrimary800 = ImVec4(0.0, 0.17, 0.11, 1.0);
constexpr ImVec4 kColorPrimary900 = ImVec4(0.0, 0.08, 0.05, 1.0);

constexpr ImVec4 kColorDanger50 = ImVec4(0.94, 0.9, 0.9, 1.0);
constexpr ImVec4 kColorDanger100 = ImVec4(0.88, 0.8, 0.8, 1.0);
constexpr ImVec4 kColorDanger200 = ImVec4(0.77, 0.6, 0.6, 1.0);
constexpr ImVec4 kColorDanger300 = ImVec4(0.65, 0.4, 0.4, 1.0);
constexpr ImVec4 kColorDanger400 = ImVec4(0.54, 0.21, 0.21, 1.0);
constexpr ImVec4 kColorDanger500 = ImVec4(0.43, 0.01, 0.01, 1.0);
constexpr ImVec4 kColorDanger600 = ImVec4(0.34, 0.01, 0.01, 1.0);
constexpr ImVec4 kColorDanger700 = ImVec4(0.25, 0.0, 0.0, 1.0);
constexpr ImVec4 kColorDanger800 = ImVec4(0.17, 0.0, 0.0, 1.0);
constexpr ImVec4 kColorDanger900 = ImVec4(0.08, 0.0, 0.0, 1.0);

constexpr ImVec4 kColorNeutral50 = ImVec4(0.98, 0.98, 0.98, 1.0);
constexpr ImVec4 kColorNeutral100 = ImVec4(0.96, 0.96, 0.96, 1.0);
constexpr ImVec4 kColorNeutral200 = ImVec4(0.90, 0.90, 0.90, 1.0);
constexpr ImVec4 kColorNeutral300 = ImVec4(0.83, 0.83, 0.83, 1.0);
constexpr ImVec4 kColorNeutral400 = ImVec4(0.64, 0.64, 0.64, 1.0);
constexpr ImVec4 kColorNeutral500 = ImVec4(0.45, 0.45, 0.45, 1.0);
constexpr ImVec4 kColorNeutral600 = ImVec4(0.32, 0.32, 0.32, 1.0);
constexpr ImVec4 kColorNeutral700 = ImVec4(0.25, 0.25, 0.25, 1.0);
constexpr ImVec4 kColorNeutral800 = ImVec4(0.15, 0.15, 0.15, 1.0);
constexpr ImVec4 kColorNeutral900 = ImVec4(0.09, 0.09, 0.09, 1.0);
constexpr ImVec4 kColorNeutral1000 = ImVec4(0.04, 0.04, 0.04, 1.0);

constexpr ImVec4 kColorBlack = ImVec4(0.04, 0.04, 0.04, 1.0);
constexpr ImVec4 kColorWhite = kColorNeutral100;

constexpr ImVec4 kColorDefaultText = kColorBlack;
constexpr ImVec4 kColorDefaultBackground = kColorWhite;

constexpr ImVec4 kColorErrorText = kColorDanger500;

constexpr ImVec4 kColorTableHeader = kColorNeutral50;
constexpr ImVec4 kColorTableBody = ImVec4(1.0, 1.0, 1.0, 1.0);

// Style
constexpr ImVec2 kStyleItemSpacing = ImVec2(8.0, 8.0);
constexpr ImVec2 kStyleScreenPadding = ImVec2(16.0, 8.0);

constexpr ImVec2 kStyleButtonPadding = ImVec2(24.0, 8.0);
constexpr float kStyleButtonRounding = 12.0;

constexpr ImVec2 kStyleInputPadding = ImVec2(16.0, 8.0);
constexpr float kStyleInputRounding = 12.0;

constexpr float kTableHeaderCellHeight = 32.0;
constexpr float kTableBodyCellHeight = 44.0;
constexpr float kTableCellPaddingX = 20.0;
constexpr float kTableRounding = 12.0;

// Backend-related
constexpr std::string_view kBackendApiUrl = "https://contrel.mixero.dev/api";
}  // namespace constants

#endif  // DESKTOP_CLIENT_CONSTANTS_H
