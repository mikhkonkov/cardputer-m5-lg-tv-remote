#pragma once
#include "ui/Screen.h"

namespace ui::screens {

// Configuration + debug. Intentionally separate from end-user remote UX.
// Two columns:
//   left  : profile selector + active info
//   right : last-tx readout + "test current" button
class SettingsScreen : public Screen {
public:
    const char* title() const override { return "SETUP"; }
    void build() override;
    void onKey(char c, bool shift) override;
    void onTick(uint32_t nowMs) override;
    void onConfirm(bool isRepeat) override;

protected:
    void drawOverlay() override;
};

} // namespace ui::screens
