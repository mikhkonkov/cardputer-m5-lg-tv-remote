// Logical command identifiers. UI/code reference these, never raw IR values.
#pragma once

#include <stdint.h>

namespace ir {

enum class CommandId : uint8_t {
    None = 0,

    // Power & mode
    Power,
    Input,
    Mute,

    // Volume / channel (repeatable)
    VolumeUp,
    VolumeDown,
    ChannelUp,
    ChannelDown,

    // Directional cluster
    Up,
    Down,
    Left,
    Right,
    Ok,

    // Navigation
    Back,
    Home,
    Exit,
    Menu,
    Settings,
    Info,

    // Digits
    Num0, Num1, Num2, Num3, Num4,
    Num5, Num6, Num7, Num8, Num9,

    // Color keys
    Red,
    Green,
    Yellow,
    Blue,

    // Media (optional, may be unmapped on some LG profiles)
    Play,
    Pause,
    Stop,
    Rewind,
    Forward,

    Count // sentinel; keep last
};

constexpr uint8_t kCommandCount = static_cast<uint8_t>(CommandId::Count);

const char* commandIdName(CommandId id);

} // namespace ir
