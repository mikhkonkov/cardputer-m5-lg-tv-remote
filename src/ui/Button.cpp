#include "ui/Button.h"

#include <M5GFX.h>

#include "ui/Theme.h"

namespace ui {

namespace {

uint16_t fillFor(BtnStyle s) {
    switch (s) {
        case BtnStyle::Power:   return Theme::kBtnPower;
        case BtnStyle::Mute:    return Theme::kBtnMute;
        case BtnStyle::Input:   return Theme::kBtnInput;
        case BtnStyle::Nav:     return Theme::kBtnNav;
        case BtnStyle::Ok:      return Theme::kBtnOk;
        case BtnStyle::Volume:  return Theme::kBtnVolume;
        case BtnStyle::Channel: return Theme::kBtnChannel;
        case BtnStyle::Digit:   return Theme::kBtnDigit;
        case BtnStyle::Red:     return Theme::kBtnColorR;
        case BtnStyle::Green:   return Theme::kBtnColorG;
        case BtnStyle::Yellow:  return Theme::kBtnColorY;
        case BtnStyle::Blue:    return Theme::kBtnColorB;
        case BtnStyle::Accent:  return Theme::kBtnAccent;
        default:                return Theme::kBtnNeutral;
    }
}

uint16_t textFor(BtnStyle s) {
    switch (s) {
        case BtnStyle::Yellow: return 0x0000;
        case BtnStyle::Green:  return 0x0000;
        default:               return Theme::kTextPrimary;
    }
}

void drawHotkeyBadge(LovyanGFX& d, int16_t x, int16_t y, const char* text) {
    if (text == nullptr || text[0] == 0) return;
    const int16_t bw = 14;
    const int16_t bh = 10;
    d.fillRoundRect(x, y, bw, bh, 2, Theme::kFocusGlow);
    d.drawRoundRect(x, y, bw, bh, 2, 0x0000);
    d.setTextColor(0x0000);
    d.setTextDatum(middle_center);
    d.setTextSize(1);
    d.drawString(text, x + bw / 2, y + bh / 2);
    d.setTextDatum(top_left);
}

} // namespace

void Button::draw(LovyanGFX& d, bool focused, bool pressed, bool defined,
                  bool showHint) const {
    const uint16_t fill   = pressed ? Theme::kPressed : fillFor(style);
    const uint16_t txt    = pressed ? 0x0000 : textFor(style);
    const uint16_t border = focused ? Theme::kFocusBorder : 0x0000;

    if (shape == BtnShape::Circle) {
        const int16_t r  = (w < h ? w : h) / 2 - 1;
        const int16_t cx = x + w / 2;
        const int16_t cy = y + h / 2;
        d.fillCircle(cx, cy, r, fill);
        if (focused) {
            d.drawCircle(cx, cy, r,     border);
            d.drawCircle(cx, cy, r - 1, border);
        }
    } else {
        d.fillRoundRect(x, y, w, h, Theme::kRadius, fill);
        if (focused) {
            d.drawRoundRect(x, y, w, h, Theme::kRadius, border);
            d.drawRoundRect(x + 1, y + 1, w - 2, h - 2, Theme::kRadius, border);
        }
    }

    if (!defined) {
        d.drawLine(x + 2, y + h - 2, x + w - 2, y + 2, Theme::kTextDim);
    }

    d.setTextColor(txt);
    d.setTextDatum(middle_center);
    d.setTextSize(1);
    const char* s = icon ? icon : label;
    d.drawString(s, x + w / 2, y + h / 2);
    d.setTextDatum(top_left);

    if (showHint && hotkey != 0) {
        char buf[3] = { hotkey, 0, 0 };
        const char* label = hotkeyLbl ? hotkeyLbl : buf;
        drawHotkeyBadge(d, x + 1, y + 1, label);
    }
}

} // namespace ui
