#include "app/ScreenManager.h"

#include "ui/Screen.h"

namespace app {

void ScreenManager::registerScreen(ScreenId id, ui::Screen* s) {
    const uint8_t i = static_cast<uint8_t>(id);
    if (i >= static_cast<uint8_t>(ScreenId::Count)) return;
    screens_[i] = s;
}

void ScreenManager::show(ScreenId id) {
    const uint8_t i = static_cast<uint8_t>(id);
    if (i >= static_cast<uint8_t>(ScreenId::Count)) return;
    ui::Screen* next = screens_[i];
    if (next == nullptr || next == current_) return;
    if (current_) current_->onLeave();
    current_   = next;
    currentId_ = id;
    current_->onEnter();
}

void ScreenManager::toggleMainExtra() {
    show(currentId_ == ScreenId::MainRemote ? ScreenId::ExtraKeys
                                            : ScreenId::MainRemote);
}

void ScreenManager::goHome() {
    show(ScreenId::MainRemote);
}

} // namespace app
