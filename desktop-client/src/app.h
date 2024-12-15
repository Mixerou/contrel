//
// Created by Mixerou on 14.12.2024.
//

#ifndef DESKTOP_CLIENT_APP_H
#define DESKTOP_CLIENT_APP_H

#include <imgui.h>

#include "workers.h"

namespace app {
extern ImFont *body_font;
extern ImFont *heading_xl_font;

extern workers::ApiWorker api_worker;

void InitFonts();

void InitStyle();
}  // namespace app

#endif  // DESKTOP_CLIENT_APP_H
