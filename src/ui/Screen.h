// Screen base. Owns a fixed array of buttons and handles 2D focus navigation
// by geometric nearest-neighbor search.
#pragma once

#include <stdint.h>

#include <M5GFX.h>

#include "ui/Button.h"

namespace ir       { class RemoteController; }
namespace profiles { class ProfileManager;    }
namespace app      { class ScreenManager;     }

namespace ui {

class Toast;

enum class Nav : uint8_t { Up, Down, Left, Right };

class Screen {
public:
    virtual ~Screen() = default;

    void attach(LovyanGFX* d,
                ir::RemoteController* rc,
                profiles::ProfileManager* pm,
                Toast* toast,
                app::ScreenManager* sm) {
        d_ = d; rc_ = rc; pm_ = pm; toast_ = toast; sm_ = sm;
    }

    virtual const char* title() const = 0;
    virtual void build()              = 0; // populate buttons_ once
    virtual void onEnter()            { needsFullRedraw_ = true; }
    virtual void onLeave()            {}

    // Default handling for common keys. Override for special behavior.
    virtual void onKey(char c, bool shift);
    virtual void onNav(Nav n);
    virtual void onConfirm(bool isRepeat);
    virtual void onTick(uint32_t nowMs);

    virtual void render(uint32_t nowMs);

    void requestFullRedraw() { needsFullRedraw_ = true; }

    void toggleHints() { showHints_ = !showHints_; needsFullRedraw_ = true; }
    bool hintsVisible() const { return showHints_; }

protected:
    static constexpr uint8_t kMaxButtons = 32;

    // ---- helpers for subclasses ----
    void clearButtons() { buttonCount_ = 0; focus_ = 0; }
    void addButton(const Button& b);
    int8_t findIdByCommand(ir::CommandId id) const;
    void setFocusById(uint8_t id);
    void sendFocused(bool isRepeat);

    // Send a command directly (independent of focus). If focusIfPresent and a
    // button on this screen carries this command, focus is moved to it and a
    // pressed flash is shown. Used by remote screens to wire arrow keys + Enter
    // to TV D-pad / OK regardless of which button is highlighted.
    void sendCommand(ir::CommandId id, bool focusIfPresent);

    // Optional - drawn before buttons. Default fills with theme bg.
    virtual void drawBackground();
    // Optional - drawn after buttons.
    virtual void drawOverlay() {}

    // ---- state ----
    LovyanGFX*                    d_      = nullptr;
    ir::RemoteController*     rc_     = nullptr;
    profiles::ProfileManager* pm_     = nullptr;
    Toast*                    toast_  = nullptr;
    app::ScreenManager*       sm_     = nullptr;

    bool tryHotkey(char c);

    Button   buttons_[kMaxButtons];
    uint8_t  buttonCount_   = 0;
    uint8_t  focus_         = 0;
    int8_t   pressedIdx_    = -1;
    uint32_t pressedUntil_  = 0;
    bool     needsFullRedraw_ = true;
    bool     showHints_       = false;

private:
    int8_t pickNeighbor(Nav n) const;
    void   drawHeader();
    void   drawFooter();
};

} // namespace ui
