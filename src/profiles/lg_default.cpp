// LG default profile - 32-bit NEC ("LG NEC32"), 38 kHz carrier, address prefix 0x20DF.
//
// Values below are the widely-published LG standard remote codes that work
// on most LG TVs from ~2010 onward (AKB7xxx / MR-series). If your model
// rejects a code, override it in this file - the rest of the firmware does
// not need to change.
//
// Source confidence:
//   HIGH    : core nav (Power, Vol, Ch, Mute, Input, arrows, OK, Back, digits)
//   MEDIUM  : Home/Menu/Exit/Settings/Info (LG has used several codes here)
//   LOW     : Color/media (varies by model line - WebOS vs. older firmwares)
//
// To verify on a specific TV, use the Settings screen "test command" feature.
#include "profiles/lg_default.h"
#include "ir/CommandId.h"
#include "ir/IrCommand.h"

namespace profiles {

using ir::CommandId;
using ir::IrCommand;

namespace {

constexpr ProfileMapping kMap[] = {
    // ----- Core power / mode (HIGH confidence) -----
    { CommandId::Power,       IrCommand::nec(0x20DF10EF, false) },
    { CommandId::Input,       IrCommand::nec(0x20DFD02F, false) },
    { CommandId::Mute,        IrCommand::nec(0x20DF906F, false) },

    // ----- Volume / channel (HIGH, repeatable) -----
    { CommandId::VolumeUp,    IrCommand::nec(0x20DF40BF, true)  },
    { CommandId::VolumeDown,  IrCommand::nec(0x20DFC03F, true)  },
    { CommandId::ChannelUp,   IrCommand::nec(0x20DF00FF, true)  },
    { CommandId::ChannelDown, IrCommand::nec(0x20DF807F, true)  },

    // ----- D-pad (HIGH) -----
    { CommandId::Up,          IrCommand::nec(0x20DF02FD, true)  },
    { CommandId::Down,        IrCommand::nec(0x20DF827D, true)  },
    { CommandId::Left,        IrCommand::nec(0x20DFE01F, true)  },
    { CommandId::Right,       IrCommand::nec(0x20DF609F, true)  },
    { CommandId::Ok,          IrCommand::nec(0x20DF22DD, false) },

    // ----- Navigation (MEDIUM - varies by model) -----
    { CommandId::Back,        IrCommand::nec(0x20DF14EB, false) },
    { CommandId::Home,        IrCommand::nec(0x20DF7E81, false) }, // WebOS "Smart Home"
    { CommandId::Exit,        IrCommand::nec(0x20DFDA25, false) },
    { CommandId::Menu,        IrCommand::nec(0x20DFC23D, false) }, // Q.Menu
    { CommandId::Settings,    IrCommand::nec(0x20DFA25D, false) }, // Settings / setup
    { CommandId::Info,        IrCommand::nec(0x20DF55AA, false) },

    // ----- Digits (HIGH) -----
    { CommandId::Num0,        IrCommand::nec(0x20DF08F7, false) },
    { CommandId::Num1,        IrCommand::nec(0x20DF8877, false) },
    { CommandId::Num2,        IrCommand::nec(0x20DF48B7, false) },
    { CommandId::Num3,        IrCommand::nec(0x20DFC837, false) },
    { CommandId::Num4,        IrCommand::nec(0x20DF28D7, false) },
    { CommandId::Num5,        IrCommand::nec(0x20DFA857, false) },
    { CommandId::Num6,        IrCommand::nec(0x20DF6897, false) },
    { CommandId::Num7,        IrCommand::nec(0x20DFE817, false) },
    { CommandId::Num8,        IrCommand::nec(0x20DF18E7, false) },
    { CommandId::Num9,        IrCommand::nec(0x20DF9867, false) },

    // ----- Color keys (LOW - verify per model) -----
    { CommandId::Red,         IrCommand::nec(0x20DF4EB1, false) },
    { CommandId::Green,       IrCommand::nec(0x20DF8E71, false) },
    { CommandId::Yellow,      IrCommand::nec(0x20DFC639, false) },
    { CommandId::Blue,        IrCommand::nec(0x20DF8679, false) },

    // ----- Media (LOW - left undefined; add if your model supports them) -----
    // { CommandId::Play,     IrCommand::nec(0x20DF0DF2, false) },
    // { CommandId::Pause,    IrCommand::nec(0x20DF5DA2, false) },
    // { CommandId::Stop,     IrCommand::nec(0x20DF8D72, false) },
};

constexpr uint8_t kMapCount = sizeof(kMap) / sizeof(kMap[0]);

} // namespace

const Profile kLgDefaultProfile{
    /*brand    */ "LG",
    /*modelHint*/ "NEC32 (most LG TVs)",
    /*mappings */ kMap,
    /*mapCount */ kMapCount,
};

} // namespace profiles
