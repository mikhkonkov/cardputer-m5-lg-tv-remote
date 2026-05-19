// Tiny stack-less screen switcher. Three slots, indexed by enum.
#pragma once

#include <stdint.h>

namespace ui { class Screen; }

namespace app {

enum class ScreenId : uint8_t {
    MainRemote = 0,
    ExtraKeys,
    Settings,
    Count
};

class ScreenManager {
public:
    void registerScreen(ScreenId id, ui::Screen* s);
    void show(ScreenId id);

    ui::Screen* current() { return current_; }
    ScreenId    currentId() const { return currentId_; }

    void toggleMainExtra();
    void goHome();

private:
    ui::Screen* screens_[static_cast<uint8_t>(ScreenId::Count)] = {nullptr};
    ui::Screen* current_   = nullptr;
    ScreenId    currentId_ = ScreenId::MainRemote;
};

} // namespace app
