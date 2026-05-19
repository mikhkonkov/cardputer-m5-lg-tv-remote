// Main remote layout. 240x135 minus 14 header / 12 footer = 109px usable.
#include "ui/screens/MainRemoteScreen.h"

#include "ir/CommandId.h"

namespace ui::screens {

using ir::CommandId;

namespace {

// Constructs a Button with id/label/x/y/w/h/command/style/shape/icon/hotkey/hotkeyLbl.
constexpr Button mk(uint8_t id, const char* lbl,
                    int16_t x, int16_t y, int16_t w, int16_t h,
                    CommandId cmd, BtnStyle style,
                    char hotkey = 0, const char* hotkeyLbl = nullptr,
                    BtnShape shape = BtnShape::Rect) {
    return Button{ id, lbl, x, y, w, h, cmd, style, shape, nullptr, hotkey, hotkeyLbl };
}

} // namespace

void MainRemoteScreen::build() {
    clearButtons();

    // ----- Top row: Power / Input / Mute -----
    addButton(mk( 0, "PWR",   4, 18, 36, 22, CommandId::Power, BtnStyle::Power, 'p'));
    addButton(mk( 1, "INP",  44, 18, 32, 22, CommandId::Input, BtnStyle::Input, 'i'));
    addButton(mk( 2, "MUTE", 80, 18, 40, 22, CommandId::Mute,  BtnStyle::Mute,  'm'));

    // ----- D-pad cluster (centered) -----
    // cy chosen so the "v" arrow (cy + 32) clears the bottom row at y=102.
    const int16_t cx = 64; // cluster center x
    const int16_t cy = 66; // cluster center y
    const int16_t s  = 22; // arrow size
    addButton(mk( 3, "^",
                  static_cast<int16_t>(cx - s/2),
                  static_cast<int16_t>(cy - s - 2),
                  s, s, CommandId::Up,    BtnStyle::Nav, 0, nullptr));
    addButton(mk( 4, "v",
                  static_cast<int16_t>(cx - s/2),
                  static_cast<int16_t>(cy + 2 + 8),
                  s, s, CommandId::Down,  BtnStyle::Nav, 0, nullptr));
    addButton(mk( 5, "<",
                  static_cast<int16_t>(cx - s - 2 - 8),
                  static_cast<int16_t>(cy - s/2 + 4),
                  s, s, CommandId::Left,  BtnStyle::Nav, 0, nullptr));
    addButton(mk( 6, ">",
                  static_cast<int16_t>(cx + 2 + 8),
                  static_cast<int16_t>(cy - s/2 + 4),
                  s, s, CommandId::Right, BtnStyle::Nav, 0, nullptr));
    addButton(mk( 7, "OK",
                  static_cast<int16_t>(cx - 14),
                  static_cast<int16_t>(cy - 10),
                  28, 24, CommandId::Ok, BtnStyle::Ok, 0, nullptr, BtnShape::Circle));

    // ----- Volume / channel column (right side) -----
    const int16_t rx = 168;
    const int16_t rw = 64;
    addButton(mk( 8, "VOL +", rx, 18, rw, 18, CommandId::VolumeUp,    BtnStyle::Volume,  '=', "="));
    addButton(mk( 9, "VOL -", rx, 38, rw, 18, CommandId::VolumeDown,  BtnStyle::Volume,  '-'));
    addButton(mk(10, "CH +",  rx, 60, rw, 18, CommandId::ChannelUp,   BtnStyle::Channel, ']'));
    addButton(mk(11, "CH -",  rx, 80, rw, 18, CommandId::ChannelDown, BtnStyle::Channel, '['));

    // ----- Lower nav row (4 buttons evenly spaced) -----
    addButton(mk(12, "BCK",    4, 102, 54, 18, CommandId::Back, BtnStyle::Neutral, 'b'));
    addButton(mk(13, "MENU",  62, 102, 54, 18, CommandId::Menu, BtnStyle::Accent,  'o'));
    addButton(mk(14, "HOME", 120, 102, 54, 18, CommandId::Home, BtnStyle::Accent,  'n'));
    addButton(mk(15, "EXIT", 178, 102, 54, 18, CommandId::Exit, BtnStyle::Neutral, 'e'));

    setFocusById(7);
}

void MainRemoteScreen::onNav(Nav n) {
    using ir::CommandId;
    CommandId id = CommandId::None;
    switch (n) {
        case Nav::Up:    id = CommandId::Up;    break;
        case Nav::Down:  id = CommandId::Down;  break;
        case Nav::Left:  id = CommandId::Left;  break;
        case Nav::Right: id = CommandId::Right; break;
    }
    sendCommand(id, true);
}

void MainRemoteScreen::onConfirm(bool isRepeat) {
    if (isRepeat) return; // TV OK is not auto-repeat
    sendCommand(ir::CommandId::Ok, true);
}

} // namespace ui::screens
