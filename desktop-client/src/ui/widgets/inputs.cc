//
// Created by Mixerou on 17.12.2024.
//

#include <imgui.h>

#include "constants.h"
#include "widgets.h"

using namespace constants;

namespace widgets {
bool InputText(const char *label, char *buf, size_t buf_size,
               ImGuiInputTextFlags flags, ImGuiInputTextCallback callback,
               void *user_data) {
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, kStyleInputPadding);
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, kStyleInputRounding);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, kColorNeutral200);

  bool input =
      ImGui::InputText(label, buf, buf_size, flags, callback, user_data);

  ImGui::PopStyleColor();
  ImGui::PopStyleVar(2);

  return input;
}

bool MetaInputText(const char *label, char *buf, size_t buf_size,
                   ImGuiInputTextFlags flags, ImGuiInputTextCallback callback,
                   void *user_data) {
  ImGui::PushID(label);
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0, 4.0));

  ImGui::Indent(kStyleInputRounding);
  widgets::BodyText("%s", label);
  ImGui::Unindent(kStyleInputRounding);

  bool input = InputText("", buf, buf_size, flags, callback, user_data);

  ImGui::PopStyleVar();
  ImGui::PopID();

  return input;
}

int FilterInputPhoneNumber(ImGuiInputTextCallbackData *data) {
  for (int i = 0; i < data->BufTextLen; i++) {
    if ((i == 0 && !strchr("0123456789+", data->Buf[i])) ||
        (i != 0 && !strchr("0123456789", data->Buf[i]))) {
      data->DeleteChars(i, 1);
    }
  }

  return 0;
}

int FilterInputDate(ImGuiInputTextCallbackData *data) {
  for (int i = 0; i < data->BufTextLen; i++) {
    if (!strchr("0123456789/", data->Buf[i])) {
      data->DeleteChars(i, 1);
      i--;
      continue;
    }

    if (data->Buf[i] == '/' && i != 2 && i != 5) {
      data->DeleteChars(i, 1);
      i--;
      continue;
    }

    if ((i == 2 || i == 5) && data->Buf[i] != '/') {
      data->InsertChars(i, "/");
      continue;
    }

    if (i == 1) {
      if (const int day = (data->Buf[0] - '0') * 10 + (data->Buf[1] - '0');
          day < 1 || day > 31) {
        data->DeleteChars(1, 1);
        i--;
        continue;
      }
    }

    if (i == 4) {
      if (int month = (data->Buf[3] - '0') * 10 + (data->Buf[4] - '0');
          month < 1 || month > 12) {
        data->DeleteChars(4, 1);
        i--;
        continue;
      }
    }

    if (i == 9) {
      const int year = (data->Buf[6] - '0') * 1000 +
                       (data->Buf[7] - '0') * 100 + (data->Buf[8] - '0') * 10 +
                       (data->Buf[9] - '0');

      if (year < 1970 || year > 2100) {
        data->DeleteChars(9, 1);
        i--;
      }
    }
  }

  return 0;
}
}  // namespace widgets
