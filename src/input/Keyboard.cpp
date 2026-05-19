// Cardputer key map:
//
//   ; , . /            arrow cluster (right side of keyboard) - sends TV D-pad
//                      via Nav events; auto-repeats while held
//   Enter              sends focused button (Confirm); base screens forward
//                      this to TV OK on the remote screens
//   `                  jump to Main Remote
//   Tab                toggle Main / Extra Keys
//   p                  power shortcut (sends Power command)
//   s                  open Setup
//   Esc                go back to Main Remote
//   h                  toggle on-screen hotkey hints (handled in Screen)
//   any letter         tries to match a button's hotkey on the current screen

#include "input/Keyboard.h"

#include <M5Cardputer.h>

#include "config.h"

namespace input {

namespace {

bool isArrowChar(char c) {
    return c == ';' || c == '.' || c == ',' || c == '/';
}

int arrowIndex(char c) {
    switch (c) {
        case ';': return 0; // Up
        case '.': return 1; // Down
        case ',': return 2; // Left
        case '/': return 3; // Right
        default:  return -1;
    }
}

} // namespace

void Keyboard::begin() {
    // M5Cardputer.begin() already initializes the keyboard.
}

bool Keyboard::processHold(HoldState& st, bool nowDown, uint32_t nowMs) {
    if (nowDown) {
        if (!st.held) {
            st.held = true;
            st.downAt = nowMs;
            st.repeating = false;
            return true; // edge: caller should mark as "press"
        }
        if (!st.repeating && (nowMs - st.downAt) >= config::kHoldRepeatDelayMs) {
            st.repeating    = true;
            st.lastRepeatAt = nowMs;
            return true;
        }
        if (st.repeating && (nowMs - st.lastRepeatAt) >= config::kHoldRepeatPeriodMs) {
            st.lastRepeatAt = nowMs;
            return true;
        }
        return false;
    }
    st.held = false;
    st.repeating = false;
    return false;
}

bool Keyboard::poll(uint32_t nowMs, Event* out) {
    M5Cardputer.update();
    Keyboard_Class::KeysState s = M5Cardputer.Keyboard.keysState();

    // ----- Snapshot which arrows are currently pressed -----
    bool arrowsNow[4] = { false, false, false, false };
    for (char c : s.word) {
        const int ai = arrowIndex(c);
        if (ai >= 0) arrowsNow[ai] = true;
    }

    // ----- Enter: highest priority (so Confirm doesn't get starved by arrows) -----
    const bool firedConfirm = processHold(confirm_, s.enter, nowMs);
    if (firedConfirm) {
        out->type = confirm_.repeating ? EventType::Repeat : EventType::Confirm;
        return true;
    }

    // ----- Arrows: same hold/repeat logic, per direction -----
    for (int i = 0; i < 4; ++i) {
        if (processHold(arrows_[i], arrowsNow[i], nowMs)) {
            out->type = EventType::Nav;
            out->nav  = static_cast<NavDir>(i);
            return true;
        }
    }

    // ----- Edge-only character events -----
    if (!M5Cardputer.Keyboard.isChange()) return false;
    if (!M5Cardputer.Keyboard.isPressed()) return false;

    for (char ch : s.word) {
        if (isArrowChar(ch)) continue; // already covered by arrow holds
        switch (ch) {
            case '`':
                out->type = EventType::Home;
                return true;
            case '\t':
                out->type = EventType::Toggle;
                return true;
            case 'p': case 'P':
                out->type = EventType::Power;
                return true;
            case 's':
                out->type = EventType::Setup;
                return true;
            case 27: // Esc, if mapped
                out->type = EventType::Escape;
                return true;
            default:
                out->type  = EventType::Char;
                out->ch    = ch;
                out->shift = s.shift;
                return true;
        }
    }
    return false;
}

} // namespace input
