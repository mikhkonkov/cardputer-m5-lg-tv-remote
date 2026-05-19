// Tiny transient status bar drawn at the bottom of the active screen.
#pragma once

#include <stdint.h>

#include <M5GFX.h>

namespace ui {

class Toast {
public:
    void show(const char* text, uint16_t color, uint32_t durationMs);
    // Returns true exactly once when the toast transitions from visible to
    // expired - the caller should use that signal to force a screen redraw
    // so the strip painted by draw() is cleared.
    bool update(uint32_t nowMs);
    void draw(LovyanGFX& d, uint32_t nowMs);

    bool active(uint32_t nowMs) const { return nowMs < expireAtMs_; }

private:
    const char* text_       = nullptr;
    uint16_t    color_      = 0xFFFF;
    uint32_t    expireAtMs_ = 0;
    uint32_t    shownAtMs_  = 0;
};

} // namespace ui
