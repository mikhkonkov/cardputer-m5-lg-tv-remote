#pragma once
#include "ui/Screen.h"

namespace ui::screens {

class ExtraKeysScreen : public Screen {
public:
    const char* title() const override { return "KEYS"; }
    void build() override;

    void onKey(char c, bool shift) override;
    void onNav(Nav n) override;
    void onConfirm(bool isRepeat) override;
};

} // namespace ui::screens
