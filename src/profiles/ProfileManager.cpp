#include "profiles/ProfileManager.h"

#include "config.h"
#include "profiles/lg_default.h"
#include "profiles/lg_alt.h"

namespace profiles {

void ProfileManager::begin() {
    registerProfile(&kLgDefaultProfile);
    registerProfile(&kLgAltProfile);
    currentIndex_ = config::kDefaultProfileIndex;
    if (currentIndex_ >= count_) currentIndex_ = 0;
}

bool ProfileManager::registerProfile(const Profile* p) {
    if (p == nullptr || count_ >= kMaxProfiles) return false;
    slots_[count_++] = p;
    return true;
}

const Profile* ProfileManager::current() const {
    if (count_ == 0) return nullptr;
    return slots_[currentIndex_];
}

const Profile* ProfileManager::at(uint8_t i) const {
    if (i >= count_) return nullptr;
    return slots_[i];
}

void ProfileManager::setIndex(uint8_t i) {
    if (i < count_) currentIndex_ = i;
}

void ProfileManager::next() {
    if (count_ == 0) return;
    currentIndex_ = (currentIndex_ + 1) % count_;
}

void ProfileManager::prev() {
    if (count_ == 0) return;
    currentIndex_ = (currentIndex_ + count_ - 1) % count_;
}

} // namespace profiles
