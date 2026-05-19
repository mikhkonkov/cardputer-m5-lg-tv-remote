#pragma once
#include "ui/Screen.h"

namespace ui::screens {

class MainRemoteScreen : public Screen {
public:
    const char* title() const override { return "REMOTE"; }
    void build() override;
    void onNav(Nav n) override;
    void onConfirm(bool isRepeat) override;
};

} // namespace ui::screens
