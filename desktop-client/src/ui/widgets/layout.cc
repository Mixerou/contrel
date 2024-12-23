//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
void SameLine(const float spacing) {
  if (spacing == 0.f) {
    return ImGui::SameLine(0.f, kStyleItemSpacing.x);
  }

  return ImGui::SameLine(0.f, spacing);
}
}  // namespace widgets
