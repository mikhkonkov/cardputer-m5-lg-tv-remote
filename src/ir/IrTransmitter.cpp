#include "ir/IrTransmitter.h"

#include <IRsend.h>
#include "config.h"

namespace ir {

namespace {
IRsend g_irsend(config::kIrLedPin);
} // namespace

IrTransmitter::IrTransmitter() = default;

void IrTransmitter::begin() {
    if (initialized_) return;
    g_irsend.begin();
    initialized_ = true;
}

bool IrTransmitter::send(const IrCommand& cmd) {
    if (!initialized_ || !cmd.defined) return false;

    switch (cmd.protocol) {
        case Protocol::NEC:
            g_irsend.sendNEC(cmd.data, cmd.bits);
            return true;
        case Protocol::Sony:
            g_irsend.sendSony(cmd.data, cmd.bits);
            return true;
        case Protocol::Raw:
            if (cmd.raw == nullptr || cmd.rawLength == 0) return false;
            g_irsend.sendRaw(cmd.raw, cmd.rawLength, 38 /* kHz */);
            return true;
        default:
            return false;
    }
}

} // namespace ir
