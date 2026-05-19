#include "ui/Toast.h"

#include <Arduino.h>
#include <M5GFX.h>

#include "config.h"
#include "ui/Theme.h"

namespace ui {

void Toast::show(const char* text, uint16_t color, uint32_t durationMs) {
    text_       = text;
    color_      = color;
    shownAtMs_  = millis();
    expireAtMs_ = shownAtMs_ + durationMs;
}

bool Toast::update(uint32_t nowMs) {
    if (text_ != nullptr && nowMs >= expireAtMs_) {
        text_ = nullptr;
        return true;
    }
    return false;
}

void Toast::draw(LovyanGFX& d, uint32_t nowMs) {
    if (text_ == nullptr || nowMs >= expireAtMs_) return;

    // Render over the footer strip so we never cover the bottom row of buttons.
    const int16_t y = config::kScreenHeight - Theme::kFooterH;
    const int16_t h = Theme::kFooterH;
    d.fillRect(0, y, config::kScreenWidth, h, color_);
    d.setTextColor(0x0000);
    d.setTextDatum(middle_center);
    d.setTextSize(1);
    d.drawString(text_, config::kScreenWidth / 2, y + h / 2);
    d.setTextDatum(top_left);
}

} // namespace ui
