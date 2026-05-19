// LG "alt" profile - placeholder for an older LG line (CRT / early plasma)
// or a regional variant. Starts as a partial clone of the default with the
// nav cluster blanked out so it visibly differs in the Settings screen.
//
// Replace these values per your hardware once tested.
#include "profiles/lg_alt.h"
#include "ir/CommandId.h"
#include "ir/IrCommand.h"

namespace profiles {

using ir::CommandId;
using ir::IrCommand;

namespace {

constexpr ProfileMapping kMap[] = {
    { CommandId::Power,       IrCommand::nec(0x20DF10EF, false) },
    { CommandId::Mute,        IrCommand::nec(0x20DF906F, false) },
    { CommandId::Input,       IrCommand::nec(0x20DFD02F, false) },

    { CommandId::VolumeUp,    IrCommand::nec(0x20DF40BF, true)  },
    { CommandId::VolumeDown,  IrCommand::nec(0x20DFC03F, true)  },
    { CommandId::ChannelUp,   IrCommand::nec(0x20DF00FF, true)  },
    { CommandId::ChannelDown, IrCommand::nec(0x20DF807F, true)  },

    { CommandId::Up,          IrCommand::nec(0x20DF02FD, true)  },
    { CommandId::Down,        IrCommand::nec(0x20DF827D, true)  },
    { CommandId::Left,        IrCommand::nec(0x20DFE01F, true)  },
    { CommandId::Right,       IrCommand::nec(0x20DF609F, true)  },
    { CommandId::Ok,          IrCommand::nec(0x20DF22DD, false) },

    // Older LG remotes used the regular "Menu" key here instead of WebOS Home.
    { CommandId::Menu,        IrCommand::nec(0x20DFC23D, false) },
    { CommandId::Back,        IrCommand::nec(0x20DF14EB, false) },
    { CommandId::Exit,        IrCommand::nec(0x20DFDA25, false) },

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
};

constexpr uint8_t kMapCount = sizeof(kMap) / sizeof(kMap[0]);

} // namespace

const Profile kLgAltProfile{
    /*brand    */ "LG",
    /*modelHint*/ "Alt / older LG",
    /*mappings */ kMap,
    /*mapCount */ kMapCount,
};

} // namespace profiles
