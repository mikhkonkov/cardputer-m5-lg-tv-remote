#include "ui/screens/SettingsScreen.h"

#include <Arduino.h>
#include <M5GFX.h>
#include <stdio.h>

#include "config.h"
#include "app/ScreenManager.h"
#include "ir/RemoteController.h"
#include "profiles/Profile.h"
#include "profiles/ProfileManager.h"
#include "ui/Theme.h"

namespace ui::screens {

using ir::CommandId;

namespace {
constexpr uint8_t kBtnPrev = 100;
constexpr uint8_t kBtnNext = 101;
constexpr uint8_t kBtnTest = 102;
constexpr uint8_t kBtnBack = 103;
} // namespace

void SettingsScreen::build() {
    clearButtons();

    // Left column: profile prev/next + test
    addButton(Button{ kBtnPrev, "<", 6,   22, 24, 22, CommandId::None,  BtnStyle::Nav });
    addButton(Button{ kBtnNext, ">", 88,  22, 24, 22, CommandId::None,  BtnStyle::Nav });
    addButton(Button{ kBtnTest, "TEST POWER", 6, 78, 106, 22, CommandId::Power, BtnStyle::Power });
    addButton(Button{ kBtnBack, "DONE",       6, 104, 106, 16, CommandId::None, BtnStyle::Accent });

    setFocusById(0);
}

void SettingsScreen::onKey(char c, bool shift) {
    if (c == '[') { if (pm_) pm_->prev(); requestFullRedraw(); return; }
    if (c == ']') { if (pm_) pm_->next(); requestFullRedraw(); return; }
    Screen::onKey(c, shift);
}

void SettingsScreen::onConfirm(bool isRepeat) {
    if (isRepeat) return; // no auto-repeat on this screen
    if (focus_ >= buttonCount_) return;
    const uint8_t id = buttons_[focus_].id;

    switch (id) {
        case kBtnPrev:
            if (pm_) pm_->prev();
            requestFullRedraw();
            return;
        case kBtnNext:
            if (pm_) pm_->next();
            requestFullRedraw();
            return;
        case kBtnBack:
            if (sm_) sm_->goHome();
            return;
        case kBtnTest:
        default:
            // Default behavior: send the focused button's command, if any.
            Screen::onConfirm(false);
            return;
    }
}

void SettingsScreen::onTick(uint32_t nowMs) {
    Screen::onTick(nowMs);
    // Force overlay area refresh once per second so the "last sent" timestamp stays current.
    static uint32_t lastTick = 0;
    if (nowMs - lastTick > 500) {
        lastTick = nowMs;
        needsFullRedraw_ = true;
    }
}

void SettingsScreen::drawOverlay() {
    if (!d_) return;

    // ----- Active profile info (top left block) -----
    char line[64];
    d_->setTextColor(Theme::kTextPrimary);
    d_->setTextDatum(top_left);
    d_->setTextSize(1);

    if (pm_) {
        const profiles::Profile* p = pm_->current();
        snprintf(line, sizeof(line), "Profile %u/%u",
                 static_cast<unsigned>(pm_->currentIndex() + 1),
                 static_cast<unsigned>(pm_->count()));
        d_->drawString(line, 36, 22);

        if (p) {
            d_->setTextColor(Theme::kTextDim);
            d_->drawString(p->brand,     36, 32);
            d_->drawString(p->modelHint, 36, 42);
        }
    }

    // Divider
    d_->drawFastVLine(120, 18, 100, Theme::kTextMuted);

    // ----- Right column: last-TX info -----
    d_->setTextColor(Theme::kTextPrimary);
    d_->drawString("LAST SENT", 128, 20);

    if (rc_) {
        const ir::LastTx& l = rc_->last();
        if (l.id == CommandId::None) {
            d_->setTextColor(Theme::kTextDim);
            d_->drawString("(none)", 128, 32);
        } else {
            d_->setTextColor(Theme::kTextDim);
            d_->drawString(ir::commandIdName(l.id), 128, 32);

            snprintf(line, sizeof(line), "0x%08lX",
                     static_cast<unsigned long>(l.data));
            d_->drawString(line, 128, 44);

            d_->setTextColor(l.ok ? Theme::kToastOk
                                  : (l.defined ? Theme::kToastFail : Theme::kTextMuted));
            const char* status = l.ok ? "OK"
                                : l.defined ? "FAIL"
                                            : "UNMAPPED";
            d_->drawString(status, 128, 56);

            const uint32_t age = (millis() - l.millis) / 1000;
            snprintf(line, sizeof(line), "%lus ago",
                     static_cast<unsigned long>(age));
            d_->setTextColor(Theme::kTextMuted);
            d_->drawString(line, 128, 68);
        }
    }

    // ----- Mapped commands count (simple coverage hint) -----
    if (pm_) {
        const profiles::Profile* p = pm_->current();
        if (p) {
            snprintf(line, sizeof(line), "%u mapped",
                     static_cast<unsigned>(p->mappingCount));
            d_->setTextColor(Theme::kTextMuted);
            d_->drawString(line, 128, 84);
        }
    }

    // Help text for setup-only keys
    d_->setTextColor(Theme::kTextMuted);
    d_->drawString("[ / ] = switch profile", 128, 100);
}

} // namespace ui::screens
