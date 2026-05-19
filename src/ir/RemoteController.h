// Bridges logical CommandId to active profile and physical transmitter.
// Records last-sent info for the Settings/Debug screen.
#pragma once

#include "ir/CommandId.h"
#include "ir/IrCommand.h"
#include "ir/IrTransmitter.h"

namespace profiles { class ProfileManager; }

namespace ir {

struct LastTx {
    CommandId id    = CommandId::None;
    bool ok         = false;
    bool defined    = false;
    uint32_t millis = 0;
    uint32_t data   = 0;
};

class RemoteController {
public:
    RemoteController(IrTransmitter& tx, profiles::ProfileManager& pm);
    void begin();

    // Returns true on successful TX. Updates last() either way.
    bool send(CommandId id);

    const LastTx& last() const { return last_; }

    // Check whether the active profile has a mapping for this id.
    bool isDefined(CommandId id) const;

private:
    IrTransmitter& tx_;
    profiles::ProfileManager& pm_;
    LastTx last_;
};

} // namespace ir
