// Colors and visual constants. RGB565.
#pragma once

#include <stdint.h>

namespace ui {

struct Theme {
    // Backgrounds
    static constexpr uint16_t kBg          = 0x0000; // black
    static constexpr uint16_t kBgPanel     = 0x10A2; // very dark blue
    static constexpr uint16_t kHeader      = 0x18C3;
    static constexpr uint16_t kFooter      = 0x10A2;

    // Text
    static constexpr uint16_t kTextPrimary = 0xFFFF;
    static constexpr uint16_t kTextDim     = 0xC618;
    static constexpr uint16_t kTextMuted   = 0x7BEF;

    // Button surfaces
    static constexpr uint16_t kBtnNeutral  = 0x39E7;
    static constexpr uint16_t kBtnNav      = 0x2945;
    static constexpr uint16_t kBtnAccent   = 0x4A89;
    static constexpr uint16_t kBtnVolume   = 0x02F2; // blue-ish
    static constexpr uint16_t kBtnChannel  = 0x0560; // green-ish
    static constexpr uint16_t kBtnDigit    = 0x2104;
    static constexpr uint16_t kBtnPower    = 0xC000; // strong red
    static constexpr uint16_t kBtnMute     = 0x9986;
    static constexpr uint16_t kBtnInput    = 0xFD80; // amber
    static constexpr uint16_t kBtnOk       = 0x4A89;
    static constexpr uint16_t kBtnColorR   = 0xF800;
    static constexpr uint16_t kBtnColorG   = 0x07E0;
    static constexpr uint16_t kBtnColorY   = 0xFFE0;
    static constexpr uint16_t kBtnColorB   = 0x001F;

    // States
    static constexpr uint16_t kFocusBorder = 0xFFFF;
    static constexpr uint16_t kFocusGlow   = 0xFE60; // amber-ish
    static constexpr uint16_t kPressed     = 0xFFFF;

    // Feedback
    static constexpr uint16_t kToastOk     = 0x07E0;
    static constexpr uint16_t kToastFail   = 0xF800;

    // Sizes
    static constexpr int16_t kHeaderH = 14;
    static constexpr int16_t kFooterH = 12;
    static constexpr int16_t kPad     = 3;
    static constexpr int16_t kRadius  = 4;
};

} // namespace ui
