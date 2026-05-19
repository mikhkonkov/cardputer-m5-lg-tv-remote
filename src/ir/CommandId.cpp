#include "ir/CommandId.h"

namespace ir {

const char* commandIdName(CommandId id) {
    switch (id) {
        case CommandId::Power:       return "Power";
        case CommandId::Input:       return "Input";
        case CommandId::Mute:        return "Mute";
        case CommandId::VolumeUp:    return "Vol+";
        case CommandId::VolumeDown:  return "Vol-";
        case CommandId::ChannelUp:   return "Ch+";
        case CommandId::ChannelDown: return "Ch-";
        case CommandId::Up:          return "Up";
        case CommandId::Down:        return "Down";
        case CommandId::Left:        return "Left";
        case CommandId::Right:       return "Right";
        case CommandId::Ok:          return "OK";
        case CommandId::Back:        return "Back";
        case CommandId::Home:        return "Home";
        case CommandId::Exit:        return "Exit";
        case CommandId::Menu:        return "Menu";
        case CommandId::Settings:    return "Settings";
        case CommandId::Info:        return "Info";
        case CommandId::Num0:        return "0";
        case CommandId::Num1:        return "1";
        case CommandId::Num2:        return "2";
        case CommandId::Num3:        return "3";
        case CommandId::Num4:        return "4";
        case CommandId::Num5:        return "5";
        case CommandId::Num6:        return "6";
        case CommandId::Num7:        return "7";
        case CommandId::Num8:        return "8";
        case CommandId::Num9:        return "9";
        case CommandId::Red:         return "Red";
        case CommandId::Green:       return "Green";
        case CommandId::Yellow:      return "Yellow";
        case CommandId::Blue:        return "Blue";
        case CommandId::Play:        return "Play";
        case CommandId::Pause:       return "Pause";
        case CommandId::Stop:        return "Stop";
        case CommandId::Rewind:      return "Rew";
        case CommandId::Forward:     return "Fwd";
        default:                     return "?";
    }
}

} // namespace ir
