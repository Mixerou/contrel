//
// Created by Mixerou on 17.12.2024.
//

#ifndef DESKTOP_CLIENT_LAYOUTS_H
#define DESKTOP_CLIENT_LAYOUTS_H

namespace layouts {
// App Layout
bool BeginAppLayout(const char *screen_heading);  // true, if made a transition
                                                  // from another screen

bool EndAppLayout();  // true, if need to prepare the screen before unmounting

// Auth Layout
bool BeginAuthLayout();

void EndAuthLayout();
}  // namespace layouts

#endif  // DESKTOP_CLIENT_LAYOUTS_H
