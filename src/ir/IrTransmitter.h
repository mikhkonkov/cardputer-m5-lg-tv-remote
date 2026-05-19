// Thin wrapper over IRremoteESP8266 IRsend. Owns the IR LED pin.
#pragma once

#include "ir/IrCommand.h"

namespace ir {

class IrTransmitter {
public:
    IrTransmitter();
    void begin();
    bool send(const IrCommand& cmd);

private:
    bool initialized_ = false;
};

} // namespace ir
