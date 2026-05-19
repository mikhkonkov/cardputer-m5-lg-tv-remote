// Keyboard polling wrapper. Turns raw key state into discrete events.
//
// Tracked specials with hold-repeat:
//   Enter      -> Confirm / Repeat
//   ; , . /    -> Nav(Up|Left|Down|Right) / Nav repeat
// Edge-only chars: everything else.
#pragma once

#include <stdint.h>

namespace input {

enum class EventType : uint8_t {
    None = 0,
    Nav,      // payload: NavDir   (arrow key, edge or repeat)
    Char,     // payload: ascii character (non-arrow)
    Confirm,  // Enter pressed (edge)
    Repeat,   // Enter held past delay
    Home,     // jump to MainRemote
    Toggle,   // toggle MainRemote <-> ExtraKeys
    Power,    // dedicated power shortcut
    Setup,    // open Settings
    Escape,
};

enum class NavDir : uint8_t { Up = 0, Down = 1, Left = 2, Right = 3 };

struct Event {
    EventType type    = EventType::None;
    NavDir    nav     = NavDir::Up;
    char      ch      = 0;
    bool      shift   = false;
};

class Keyboard {
public:
    void begin();
    bool poll(uint32_t nowMs, Event* out);

private:
    struct HoldState {
        bool     held         = false;
        uint32_t downAt       = 0;
        uint32_t lastRepeatAt = 0;
        bool     repeating    = false;
    };

    // Returns true and fills *out if an edge or repeat fires.
    bool processHold(HoldState& st, bool nowDown, uint32_t nowMs);

    HoldState confirm_;
    HoldState arrows_[4]; // index = NavDir
};

} // namespace input
