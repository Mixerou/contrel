//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_CONSTANTS_H
#define DESKTOP_CLIENT_CONSTANTS_H

#include "imgui.h"

namespace constants {
// Scale
constexpr float kFontScaleFactor = 1.0;
constexpr float kFontSizeFactor = 1.0;

// Scale (for the HiDPI monitors)
// constexpr float kFontScaleFactor = 0.5;
// constexpr float kFontSizeFactor = 2.0;

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

constexpr ImVec4 kColorBlack = ImVec4(0.04, 0.04, 0.04, 1.0);
constexpr ImVec4 kColorWhite = ImVec4(0.96, 0.96, 0.96, 1.0);

constexpr ImVec4 kColorDefaultText = kColorBlack;
constexpr ImVec4 kColorDefaultBackground = kColorWhite;
}  // namespace constants

#endif  // DESKTOP_CLIENT_CONSTANTS_H
