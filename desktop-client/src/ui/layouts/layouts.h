//
// Created by Mixerou on 17.12.2024.
//

#ifndef DESKTOP_CLIENT_LAYOUTS_H
#define DESKTOP_CLIENT_LAYOUTS_H

namespace layouts {
// App Layout
void BeginAppLayout(const char *screen_heading);

void EndAppLayout();

// Auth Layout
bool BeginAuthLayout();

void EndAuthLayout();
}  // namespace layouts

#endif  // DESKTOP_CLIENT_LAYOUTS_H
