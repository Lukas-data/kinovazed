#include "comm/Notification.h"

#include "support/EnumUtils.h"
#include "support/ToString.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>

namespace KinovaZED::Comm {

auto constexpr idNames = std::array{
    std::pair{Notification::Id::ModeSet, "ModeSet"},
    std::pair{Notification::Id::ObjectiveDone, "ObjectiveDone"},
    std::pair{Notification::Id::Unfolded, "Unfolded"},
    std::pair{Notification::Id::Retracted, "Retracted"},
};

static_assert(enumNameMappingsAreUnique(idNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(idNames, Notification::Id::ModeSet), "Missing entry in name map!");

Notification::Notification(Id id)
    : id{id} {
}

auto Notification::getId() const noexcept -> Id {
	return id;
}

auto isKnownNotificationId(int candidate) -> bool {
	return candidate >= 0 && candidate < static_cast<int>(Notification::Id::END_OF_ENUM);
}

} // namespace KinovaZED::Comm

namespace KinovaZED {

using namespace Comm;

template<>
auto toString(Notification const &notification) -> std::string {
	return toString(notification.getId());
}

template<>
auto toString(Notification::Id const &id) -> std::string {
	assert(isKnownNotificationId(static_cast<std::underlying_type_t<Notification::Id>>(id)));

	auto found = std::find_if(cbegin(idNames), cend(idNames), [&](auto entry) { return entry.first == id; });
	return found->second;
}


} // namespace KinovaZED