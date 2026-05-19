// Virtual remote button. Renders itself given a Display + state.
#pragma once

#include <stdint.h>

#include <M5GFX.h>

#include "ir/CommandId.h"

namespace ui {

enum class BtnStyle : uint8_t {
    Neutral,
    Power,
    Mute,
    Input,
    Nav,
    Ok,
    Volume,
    Channel,
    Digit,
    Red,
    Green,
    Yellow,
    Blue,
    Accent,
};

enum class BtnShape : uint8_t {
    Rect,
    Circle, // square rect rendered as a circle inscribed
};

struct Button {
    uint8_t       id        = 0;
    const char*   label     = "";
    int16_t       x         = 0;
    int16_t       y         = 0;
    int16_t       w         = 0;
    int16_t       h         = 0;
    ir::CommandId command   = ir::CommandId::None;
    BtnStyle      style     = BtnStyle::Neutral;
    BtnShape      shape     = BtnShape::Rect;
    const char*   icon      = nullptr; // small glyph drawn instead of label, if set
    char          hotkey    = 0;       // Cardputer key that triggers this button (0 = none)
    const char*   hotkeyLbl = nullptr; // visible badge text; defaults to {hotkey} if set

    void draw(LovyanGFX& d, bool focused, bool pressed, bool defined,
              bool showHint) const;
};

} // namespace ui
