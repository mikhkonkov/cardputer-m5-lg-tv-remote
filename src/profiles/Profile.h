// Profile = one TV-brand/model command table. Keep tables flat and editable.
#pragma once

#include "ir/CommandId.h"
#include "ir/IrCommand.h"

namespace profiles {

struct ProfileMapping {
    ir::CommandId id;
    ir::IrCommand cmd;
};

struct Profile {
    const char* brand;        // e.g. "LG"
    const char* modelHint;    // e.g. "NEC32 (most LG TVs)"
    const ProfileMapping* mappings;
    uint8_t mappingCount;

    // Returns IrCommand::undefined() when id is not in the table.
    ir::IrCommand lookup(ir::CommandId id) const {
        for (uint8_t i = 0; i < mappingCount; ++i) {
            if (mappings[i].id == id) return mappings[i].cmd;
        }
        return ir::IrCommand::undefined();
    }
};

} // namespace profiles
