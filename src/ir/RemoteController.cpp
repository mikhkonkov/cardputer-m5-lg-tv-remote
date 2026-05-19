#include "ir/RemoteController.h"

#include <Arduino.h>

#include "profiles/Profile.h"
#include "profiles/ProfileManager.h"

namespace ir {

RemoteController::RemoteController(IrTransmitter& tx, profiles::ProfileManager& pm)
    : tx_(tx), pm_(pm) {}

void RemoteController::begin() {
    tx_.begin();
}

bool RemoteController::isDefined(CommandId id) const {
    const profiles::Profile* p = pm_.current();
    if (!p) return false;
    return p->lookup(id).defined;
}

bool RemoteController::send(CommandId id) {
    last_.id      = id;
    last_.millis  = millis();
    last_.ok      = false;
    last_.defined = false;
    last_.data    = 0;

    const profiles::Profile* p = pm_.current();
    if (!p) return false;

    IrCommand cmd  = p->lookup(id);
    last_.defined  = cmd.defined;
    last_.data     = cmd.data;
    if (!cmd.defined) return false;

    last_.ok = tx_.send(cmd);
    return last_.ok;
}

} // namespace ir
