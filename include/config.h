// Project-wide tunable constants. Edit here, not in modules.
#pragma once

#include <stdint.h>

namespace config {

// ===== Hardware =====
// M5Cardputer IR LED pin. Verified on hardware revs shipped 2023+.
constexpr uint16_t kIrLedPin = 44;

// Display geometry after rotation(1).
constexpr int16_t kScreenWidth  = 240;
constexpr int16_t kScreenHeight = 135;

// ===== Timing (milliseconds) =====
// Time a held key must be down before repeat begins.
constexpr uint32_t kHoldRepeatDelayMs = 380;
// Period between repeats while held (volume / channel).
constexpr uint32_t kHoldRepeatPeriodMs = 140;
// Debounce window for press detection.
constexpr uint32_t kDebounceMs = 25;
// Toast visible duration after IR transmit.
constexpr uint32_t kToastMs = 380;
// Pressed-state animation hold time on a button.
constexpr uint32_t kPressFlashMs = 110;

// ===== Defaults =====
constexpr uint8_t kDefaultProfileIndex = 0;
constexpr uint8_t kDefaultBrightness   = 180; // 0..255

} // namespace config
