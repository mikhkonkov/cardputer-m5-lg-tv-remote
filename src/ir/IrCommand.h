// Physical IR command record. One row per logical command in a profile.
#pragma once

#include <stdint.h>

namespace ir {

enum class Protocol : uint8_t {
    NEC = 0,   // 32-bit LG NEC
    Sony,      // SIRC 12/15/20
    Raw,       // raw timings
    None
};

struct IrCommand {
    Protocol protocol;
    uint32_t data;       // protocol payload (NEC: 32-bit code)
    uint16_t bits;       // protocol bit count
    const uint16_t* raw; // raw timings array (Raw protocol only)
    uint16_t rawLength;
    bool repeatable;     // true for Vol/Ch holds; false for Power etc.
    bool defined;        // false = profile has no mapping for this command

    static constexpr IrCommand undefined() {
        return IrCommand{Protocol::None, 0, 0, nullptr, 0, false, false};
    }

    static constexpr IrCommand nec(uint32_t code, bool repeatable_ = false) {
        return IrCommand{Protocol::NEC, code, 32, nullptr, 0, repeatable_, true};
    }
};

} // namespace ir
