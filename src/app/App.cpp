#include "app/App.h"

#include <M5Cardputer.h>

#include "config.h"
#include "ir/CommandId.h"
#include "ui/Screen.h"

namespace app {

void App::setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true /* enableKeyboard */);
    M5Cardputer.Display.setRotation(1);
    M5Cardputer.Display.setBrightness(config::kDefaultBrightness);
    M5Cardputer.Display.fillScreen(0x0000);

    // Off-screen sprite for flicker-free double buffering.
    canvas_.setPsram(false);
    canvas_.setColorDepth(16);
    canvas_.createSprite(config::kScreenWidth, config::kScreenHeight);
    canvas_.setTextSize(1);

    pm_.begin();
    rc_.begin();
    kb_.begin();

    mainScreen_.attach (&canvas_, &rc_, &pm_, &toast_, &sm_);
    extraScreen_.attach(&canvas_, &rc_, &pm_, &toast_, &sm_);
    setupScreen_.attach(&canvas_, &rc_, &pm_, &toast_, &sm_);

    mainScreen_.build();
    extraScreen_.build();
    setupScreen_.build();

    sm_.registerScreen(ScreenId::MainRemote, &mainScreen_);
    sm_.registerScreen(ScreenId::ExtraKeys,  &extraScreen_);
    sm_.registerScreen(ScreenId::Settings,   &setupScreen_);

    sm_.show(ScreenId::MainRemote);
}

void App::dispatch(const input::Event& ev) {
    ui::Screen* s = sm_.current();
    if (s == nullptr) return;

    switch (ev.type) {
        case input::EventType::Nav: {
            using input::NavDir;
            ui::Nav n = ui::Nav::Up;
            switch (ev.nav) {
                case NavDir::Up:    n = ui::Nav::Up;    break;
                case NavDir::Down:  n = ui::Nav::Down;  break;
                case NavDir::Left:  n = ui::Nav::Left;  break;
                case NavDir::Right: n = ui::Nav::Right; break;
            }
            s->onNav(n);
            return;
        }
        case input::EventType::Confirm:
            s->onConfirm(false);
            return;
        case input::EventType::Repeat:
            s->onConfirm(true);
            return;
        case input::EventType::Home:
            sm_.show(ScreenId::MainRemote);
            return;
        case input::EventType::Toggle:
            sm_.toggleMainExtra();
            return;
        case input::EventType::Setup:
            sm_.show(ScreenId::Settings);
            return;
        case input::EventType::Escape:
            sm_.show(ScreenId::MainRemote);
            return;
        case input::EventType::Power:
            // Power shortcut works from any screen - send immediately and flash a toast.
            {
                const bool ok = rc_.send(ir::CommandId::Power);
                toast_.show("Power", ok ? 0x07E0 : 0xF800, config::kToastMs);
                s->requestFullRedraw();
            }
            return;
        case input::EventType::Char:
            s->onKey(ev.ch, ev.shift);
            return;
        case input::EventType::None:
        default:
            return;
    }
}

void App::loop() {
    const uint32_t now = millis();

    input::Event ev;
    if (kb_.poll(now, &ev)) {
        dispatch(ev);
    }

    ui::Screen* s = sm_.current();
    const bool toastExpired = toast_.update(now);
    if (s && toastExpired) s->requestFullRedraw();
    if (s) {
        s->onTick(now);
        s->render(now);
        toast_.draw(canvas_, now);
        canvas_.pushSprite(&M5Cardputer.Display, 0, 0);
    }

    delay(5); // gentle yield - keeps loop ~200 Hz
}

} // namespace app
