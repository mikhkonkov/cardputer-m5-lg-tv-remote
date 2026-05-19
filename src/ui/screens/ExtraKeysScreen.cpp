// Extra keys: digits, color buttons, info/exit/settings.
//
//   +-------------------------------------------------------+
//   |  1   2   3        INFO                                |
//   |  4   5   6        EXIT                                |
//   |  7   8   9        SET                                 |
//   |      0            R  G  Y  B                          |
//   +-------------------------------------------------------+
//
#include "ui/screens/ExtraKeysScreen.h"

#include "ir/CommandId.h"

namespace ui::screens {

using ir::CommandId;

void ExtraKeysScreen::build() {
    clearButtons();

    // ----- Digit grid (3 cols x 4 rows) -----
    const int16_t bx = 6;
    const int16_t by = 20;
    const int16_t w  = 28;
    const int16_t h  = 22;
    const int16_t gx = 4;
    const int16_t gy = 3;

    const CommandId digits[3][3] = {
        { CommandId::Num1, CommandId::Num2, CommandId::Num3 },
        { CommandId::Num4, CommandId::Num5, CommandId::Num6 },
        { CommandId::Num7, CommandId::Num8, CommandId::Num9 },
    };
    const char* digitLabels[3][3] = {
        { "1", "2", "3" }, { "4", "5", "6" }, { "7", "8", "9" }
    };

    uint8_t id = 0;
    for (uint8_t row = 0; row < 3; ++row) {
        for (uint8_t col = 0; col < 3; ++col) {
            addButton(Button{
                id++, digitLabels[row][col],
                static_cast<int16_t>(bx + col * (w + gx)),
                static_cast<int16_t>(by + row * (h + gy)),
                w, h,
                digits[row][col], BtnStyle::Digit
            });
        }
    }
    // 0 centered under 8
    addButton(Button{ id++, "0",
        static_cast<int16_t>(bx + 1 * (w + gx)),
        static_cast<int16_t>(by + 3 * (h + gy)),
        w, h, CommandId::Num0, BtnStyle::Digit });

    // ----- Right column: Info / Exit / Settings -----
    const int16_t rx = 122;
    const int16_t rw = 56;
    addButton(Button{ id++, "INFO", rx, 20, rw, 22, CommandId::Info,     BtnStyle::Accent,  BtnShape::Rect, nullptr, 'f' });
    addButton(Button{ id++, "EXIT", rx, 45, rw, 22, CommandId::Exit,     BtnStyle::Neutral, BtnShape::Rect, nullptr, 'e' });
    addButton(Button{ id++, "MENU", rx, 70, rw, 22, CommandId::Menu,     BtnStyle::Accent,  BtnShape::Rect, nullptr, 'q' });

    // ----- Color row (R G Y B) -----
    const int16_t cy = 95;
    const int16_t cw = 12;
    const int16_t ch = 22;
    addButton(Button{ id++, " ", 184, cy, cw, ch, CommandId::Red,    BtnStyle::Red,    BtnShape::Rect, nullptr, 'r' });
    addButton(Button{ id++, " ", 199, cy, cw, ch, CommandId::Green,  BtnStyle::Green,  BtnShape::Rect, nullptr, 'g' });
    addButton(Button{ id++, " ", 214, cy, cw, ch, CommandId::Yellow, BtnStyle::Yellow, BtnShape::Rect, nullptr, 'y' });
    addButton(Button{ id++, " ", 229, cy, cw, ch, CommandId::Blue,   BtnStyle::Blue,   BtnShape::Rect, nullptr, 'u' });

    setFocusById(4); // start on '5' (center digit)
}

void ExtraKeysScreen::onNav(Nav n) {
    using ir::CommandId;
    CommandId id = CommandId::None;
    switch (n) {
        case Nav::Up:    id = CommandId::Up;    break;
        case Nav::Down:  id = CommandId::Down;  break;
        case Nav::Left:  id = CommandId::Left;  break;
        case Nav::Right: id = CommandId::Right; break;
    }
    sendCommand(id, false);
}

void ExtraKeysScreen::onConfirm(bool isRepeat) {
    if (isRepeat) return;
    sendCommand(ir::CommandId::Ok, false);
}

void ExtraKeysScreen::onKey(char c, bool shift) {
    if (c >= '0' && c <= '9') {
        const CommandId map[10] = {
            CommandId::Num0, CommandId::Num1, CommandId::Num2, CommandId::Num3,
            CommandId::Num4, CommandId::Num5, CommandId::Num6, CommandId::Num7,
            CommandId::Num8, CommandId::Num9
        };
        CommandId target = map[c - '0'];
        int8_t idx = findIdByCommand(target);
        if (idx >= 0) {
            focus_ = static_cast<uint8_t>(idx);
            sendFocused(false);
        }
        return;
    }
    Screen::onKey(c, shift); // handles 'h' (toggle hints) + other hotkey matches
}

} // namespace ui::screens
