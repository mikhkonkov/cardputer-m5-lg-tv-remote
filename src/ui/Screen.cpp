#include "ui/Screen.h"

#include <Arduino.h>
#include <M5GFX.h>

#include "config.h"
#include "ir/RemoteController.h"
#include "profiles/Profile.h"
#include "profiles/ProfileManager.h"
#include "ui/Theme.h"
#include "ui/Toast.h"

namespace ui {

void Screen::addButton(const Button& b) {
    if (buttonCount_ < kMaxButtons) {
        buttons_[buttonCount_++] = b;
    }
}

int8_t Screen::findIdByCommand(ir::CommandId id) const {
    for (uint8_t i = 0; i < buttonCount_; ++i) {
        if (buttons_[i].command == id) return static_cast<int8_t>(i);
    }
    return -1;
}

void Screen::setFocusById(uint8_t id) {
    if (id < buttonCount_) {
        focus_ = id;
        needsFullRedraw_ = true;
    }
}

void Screen::onKey(char c, bool /*shift*/) {
    if (c == 'h' || c == 'H') {
        toggleHints();
        return;
    }
    tryHotkey(c);
}

bool Screen::tryHotkey(char c) {
    if (c == 0) return false;
    for (uint8_t i = 0; i < buttonCount_; ++i) {
        if (buttons_[i].hotkey != 0 && buttons_[i].hotkey == c) {
            focus_ = i;
            sendFocused(false);
            return true;
        }
    }
    return false;
}

void Screen::onConfirm(bool isRepeat) {
    sendFocused(isRepeat);
}

void Screen::sendCommand(ir::CommandId id, bool focusIfPresent) {
    if (!rc_ || id == ir::CommandId::None) return;

    int8_t idx = -1;
    if (focusIfPresent) idx = findIdByCommand(id);

    const bool ok = rc_->send(id);

    if (idx >= 0) {
        focus_        = static_cast<uint8_t>(idx);
        pressedIdx_   = idx;
        pressedUntil_ = millis() + config::kPressFlashMs;
    }
    if (toast_) {
        toast_->show(ir::commandIdName(id),
                     ok ? Theme::kToastOk : Theme::kToastFail,
                     config::kToastMs);
    }
    needsFullRedraw_ = true;
}

void Screen::sendFocused(bool isRepeat) {
    if (focus_ >= buttonCount_ || rc_ == nullptr) return;
    const Button& b = buttons_[focus_];
    if (b.command == ir::CommandId::None) return;

    // For holds, only resend repeatable commands.
    if (isRepeat) {
        const profiles::Profile* p = pm_->current();
        if (!p) return;
        if (!p->lookup(b.command).repeatable) return;
    }

    const bool ok = rc_->send(b.command);
    pressedIdx_   = static_cast<int8_t>(focus_);
    pressedUntil_ = millis() + config::kPressFlashMs;
    if (toast_) {
        toast_->show(b.label, ok ? Theme::kToastOk : Theme::kToastFail,
                     config::kToastMs);
    }
}

int8_t Screen::pickNeighbor(Nav n) const {
    if (buttonCount_ == 0) return -1;
    if (focus_ >= buttonCount_) return 0;
    const Button& cur = buttons_[focus_];
    const int16_t cx = cur.x + cur.w / 2;
    const int16_t cy = cur.y + cur.h / 2;

    int8_t best = -1;
    int32_t bestScore = INT32_MAX;

    for (uint8_t i = 0; i < buttonCount_; ++i) {
        if (i == focus_) continue;
        const Button& b = buttons_[i];
        const int16_t bx = b.x + b.w / 2;
        const int16_t by = b.y + b.h / 2;
        const int16_t dx = bx - cx;
        const int16_t dy = by - cy;

        bool valid = false;
        int32_t along = 0, perp = 0;
        switch (n) {
            case Nav::Up:    valid = dy < -2; along = -dy; perp = (dx < 0 ? -dx : dx); break;
            case Nav::Down:  valid = dy >  2; along =  dy; perp = (dx < 0 ? -dx : dx); break;
            case Nav::Left:  valid = dx < -2; along = -dx; perp = (dy < 0 ? -dy : dy); break;
            case Nav::Right: valid = dx >  2; along =  dx; perp = (dy < 0 ? -dy : dy); break;
        }
        if (!valid) continue;

        // Score: prefer small along + small perpendicular, with perp weighted heavier.
        const int32_t score = along + perp * 3;
        if (score < bestScore) {
            bestScore = score;
            best = static_cast<int8_t>(i);
        }
    }
    return best;
}

void Screen::onNav(Nav n) {
    const int8_t idx = pickNeighbor(n);
    if (idx >= 0) {
        focus_ = static_cast<uint8_t>(idx);
        needsFullRedraw_ = true;
    }
}

void Screen::onTick(uint32_t nowMs) {
    if (pressedIdx_ >= 0 && nowMs >= pressedUntil_) {
        pressedIdx_ = -1;
        needsFullRedraw_ = true;
    }
}

void Screen::drawBackground() {
    if (!d_) return;
    d_->fillRect(0, Theme::kHeaderH,
                 config::kScreenWidth,
                 config::kScreenHeight - Theme::kHeaderH - Theme::kFooterH,
                 Theme::kBg);
}

void Screen::drawHeader() {
    if (!d_) return;
    d_->fillRect(0, 0, config::kScreenWidth, Theme::kHeaderH, Theme::kHeader);
    d_->setTextColor(Theme::kTextPrimary);
    d_->setTextDatum(middle_left);
    d_->setTextSize(1);
    d_->drawString(title(), 4, Theme::kHeaderH / 2);
    if (pm_) {
        const profiles::Profile* p = pm_->current();
        if (p) {
            d_->setTextDatum(middle_right);
            d_->setTextColor(Theme::kTextDim);
            d_->drawString(p->brand, config::kScreenWidth - 4, Theme::kHeaderH / 2);
        }
    }
    d_->setTextDatum(top_left);
}

void Screen::drawFooter() {
    if (!d_) return;
    const int16_t y = config::kScreenHeight - Theme::kFooterH;
    d_->fillRect(0, y, config::kScreenWidth, Theme::kFooterH, Theme::kFooter);
    d_->setTextColor(Theme::kTextMuted);
    d_->setTextDatum(middle_left);
    d_->setTextSize(1);
    d_->drawString("arrows=Dpad  Enter=OK  h=hints  Tab=page  s=setup",
                   4, y + Theme::kFooterH / 2);
    d_->setTextDatum(top_left);
}

void Screen::render(uint32_t nowMs) {
    if (!d_) return;

    if (needsFullRedraw_) {
        d_->fillScreen(Theme::kBg);
        drawHeader();
        drawBackground();
        drawFooter();
    }

    for (uint8_t i = 0; i < buttonCount_; ++i) {
        const Button& b = buttons_[i];
        const bool focused = (i == focus_);
        const bool pressed = (pressedIdx_ == static_cast<int8_t>(i)
                              && nowMs < pressedUntil_);
        const bool defined = (rc_ && b.command != ir::CommandId::None)
                                ? rc_->isDefined(b.command)
                                : (b.command == ir::CommandId::None);
        b.draw(*d_, focused, pressed, defined, showHints_);
    }

    drawOverlay();
    needsFullRedraw_ = false;
}

} // namespace ui
