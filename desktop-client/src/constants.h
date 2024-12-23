//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_CONSTANTS_H
#define DESKTOP_CLIENT_CONSTANTS_H

#include <string>

#include <imgui.h>

namespace constants {
// Scale
constexpr float kFontScaleFactor = 1.f;
constexpr float kFontSizeFactor = 1.f;
constexpr float kVectorTextureScaleFactor = 1.F;

// Scale (for the HiDPI monitors)
// constexpr float kFontScaleFactor = 0.5f;
// constexpr float kFontSizeFactor = 2.f;
// constexpr float kVectorTextureScaleFactor = 2.f;

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
constexpr auto kColorPrimary50 = ImVec4(0.9f, 0.94f, 0.93f, 1.f);
constexpr auto kColorPrimary100 = ImVec4(0.8f, 0.88f, 0.86f, 1.f);
constexpr auto kColorPrimary200 = ImVec4(0.6f, 0.77f, 0.71f, 1.f);
constexpr auto kColorPrimary300 = ImVec4(0.4f, 0.65f, 0.57f, 1.f);
constexpr auto kColorPrimary400 = ImVec4(0.21f, 0.54f, 0.42f, 1.f);
constexpr auto kColorPrimary500 = ImVec4(0.01f, 0.42f, 0.28f, 1.f);
constexpr auto kColorPrimary600 = ImVec4(0.01f, 0.34f, 0.22f, 1.f);
constexpr auto kColorPrimary700 = ImVec4(0.f, 0.25f, 0.17f, 1.f);
constexpr auto kColorPrimary800 = ImVec4(0.f, 0.17f, 0.11f, 1.f);
constexpr auto kColorPrimary900 = ImVec4(0.f, 0.08f, 0.05f, 1.f);

constexpr auto kColorDanger50 = ImVec4(0.94f, 0.9f, 0.9f, 1.f);
constexpr auto kColorDanger100 = ImVec4(0.88f, 0.8f, 0.8f, 1.f);
constexpr auto kColorDanger200 = ImVec4(0.77f, 0.6f, 0.6f, 1.f);
constexpr auto kColorDanger300 = ImVec4(0.65f, 0.4f, 0.4f, 1.f);
constexpr auto kColorDanger400 = ImVec4(0.54f, 0.21f, 0.21f, 1.f);
constexpr auto kColorDanger500 = ImVec4(0.43f, 0.01f, 0.01f, 1.f);
constexpr auto kColorDanger600 = ImVec4(0.34f, 0.01f, 0.01f, 1.f);
constexpr auto kColorDanger700 = ImVec4(0.25f, 0.f, 0.f, 1.f);
constexpr auto kColorDanger800 = ImVec4(0.17f, 0.f, 0.f, 1.f);
constexpr auto kColorDanger900 = ImVec4(0.08f, 0.f, 0.f, 1.f);

constexpr auto kColorNeutral50 = ImVec4(0.98f, 0.98f, 0.98f, 1.f);
constexpr auto kColorNeutral100 = ImVec4(0.96f, 0.96f, 0.96f, 1.f);
constexpr auto kColorNeutral200 = ImVec4(0.90f, 0.90f, 0.90f, 1.f);
constexpr auto kColorNeutral300 = ImVec4(0.83f, 0.83f, 0.83f, 1.f);
constexpr auto kColorNeutral400 = ImVec4(0.64f, 0.64f, 0.64f, 1.f);
constexpr auto kColorNeutral500 = ImVec4(0.45f, 0.45f, 0.45f, 1.f);
constexpr auto kColorNeutral600 = ImVec4(0.32f, 0.32f, 0.32f, 1.f);
constexpr auto kColorNeutral700 = ImVec4(0.25f, 0.25f, 0.25f, 1.f);
constexpr auto kColorNeutral800 = ImVec4(0.15f, 0.15f, 0.15f, 1.f);
constexpr auto kColorNeutral900 = ImVec4(0.09f, 0.09f, 0.09f, 1.f);
constexpr auto kColorNeutral1000 = ImVec4(0.04f, 0.04f, 0.04f, 1.f);

constexpr auto kColorBlack = ImVec4(0.04f, 0.04f, 0.04f, 1.f);
constexpr auto kColorWhite = kColorNeutral100;

constexpr auto kColorDefaultText = kColorBlack;
constexpr auto kColorDefaultBackground = kColorWhite;

constexpr auto kColorErrorText = kColorDanger500;

constexpr auto kColorTableHeader = kColorNeutral50;
constexpr auto kColorTableBody = ImVec4(1.f, 1.f, 1.f, 1.f);

// Style
constexpr auto kStyleItemSpacing = ImVec2(8.f, 8.f);
constexpr auto kStyleScreenPadding = ImVec2(16.f, 8.f);

constexpr auto kStyleButtonPadding = ImVec2(24.f, 8.f);
constexpr float kStyleButtonRounding = 12.f;

constexpr auto kStyleInputPadding = ImVec2(16.f, 8.f);
constexpr float kStyleInputRounding = 12.f;

constexpr auto kStyleComboPadding = ImVec2(16.f, 8.f);
constexpr float kStyleComboRounding = 12.f;
constexpr float kStyleComboPopupRounding = 12.f;

constexpr float kTableHeaderCellHeight = 32.f;
constexpr float kTableBodyCellHeight = 44.f;
constexpr float kTableCellPaddingX = 20.f;
constexpr float kTableRounding = 12.f;

// Backend-related
constexpr std::string_view kBackendApiUrl = "https://contrel.mixero.dev/api";
}  // namespace constants

#endif  // DESKTOP_CLIENT_CONSTANTS_H
