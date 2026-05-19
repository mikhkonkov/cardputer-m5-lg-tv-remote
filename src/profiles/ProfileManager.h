// Registry of compiled-in profiles. Holds the active selection.
#pragma once

#include <stdint.h>

#include "profiles/Profile.h"

namespace profiles {

constexpr uint8_t kMaxProfiles = 4;

class ProfileManager {
public:
    void begin(); // registers built-in profiles
    bool registerProfile(const Profile* p);

    const Profile* current() const;
    uint8_t currentIndex() const { return currentIndex_; }
    uint8_t count() const { return count_; }
    const Profile* at(uint8_t i) const;

    void setIndex(uint8_t i);
    void next();
    void prev();

private:
    const Profile* slots_[kMaxProfiles] = {nullptr};
    uint8_t count_ = 0;
    uint8_t currentIndex_ = 0;
};

} // namespace profiles
