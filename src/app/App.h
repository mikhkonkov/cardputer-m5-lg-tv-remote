// Top-level app object. Owns display, input, IR, profiles, screens, toast.
#pragma once

#include <M5GFX.h>

#include "app/ScreenManager.h"
#include "input/Keyboard.h"
#include "ir/IrTransmitter.h"
#include "ir/RemoteController.h"
#include "profiles/ProfileManager.h"
#include "ui/Toast.h"
#include "ui/screens/ExtraKeysScreen.h"
#include "ui/screens/MainRemoteScreen.h"
#include "ui/screens/SettingsScreen.h"

namespace app {

class App {
public:
    void setup();
    void loop();

private:
    void dispatch(const input::Event& ev);

    // ---- subsystems ----
    profiles::ProfileManager pm_;
    ir::IrTransmitter        tx_;
    ir::RemoteController     rc_{tx_, pm_};
    input::Keyboard          kb_;
    ui::Toast                toast_;

    // ---- off-screen canvas (double buffer) -----
    M5Canvas                 canvas_; // initialized in setup() with display ptr

    // ---- screens ----
    ScreenManager                   sm_;
    ui::screens::MainRemoteScreen   mainScreen_;
    ui::screens::ExtraKeysScreen    extraScreen_;
    ui::screens::SettingsScreen     setupScreen_;
};

} // namespace app
