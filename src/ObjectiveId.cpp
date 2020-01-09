#include "ObjectiveId.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace Kinova {

auto constexpr ObjectiveNames = std::array{
    // clang-format off

	std::pair{ObjectiveId::Antenna, "Antenna"},
	std::pair{ObjectiveId::AntennaPull, "AntennaPull"},
	std::pair{ObjectiveId::Bell, "Bell"},
	std::pair{ObjectiveId::Handle, "Handle"},
	std::pair{ObjectiveId::Home, "Home"},
	std::pair{ObjectiveId::None, "NoObjective"},
	std::pair{ObjectiveId::OpenDoor, "OpenDoor"},
	std::pair{ObjectiveId::PlaceCup, "PlaceCup"},
	std::pair{ObjectiveId::PullDoor, "PullDoor"},

    // clang-format on
};

template<typename MapType>
auto constexpr HasDuplicateEntries(MapType const &map) {
	for (auto needle : map) {
		auto occurrences{0};
		for (auto entry : map) {
			occurrences += static_cast<decltype(occurrences)>(entry.first == needle.first);
		}
		if (occurrences > 1) {
			return true;
		}
	}
	return false;
}

static_assert(ObjectiveNames.size() == NumberOfObjectives, "Not all objectives have a name!");
static_assert(!HasDuplicateEntries(ObjectiveNames), "Duplicate key in objective name map!");

auto getObjectiveName(ObjectiveId id) -> std::string {
	auto entry = std::find_if(cbegin(ObjectiveNames), cend(ObjectiveNames), [&](auto e) { return e.first == id; });
	return entry->second;
}

auto getObjectiveId(std::string name) -> ObjectiveId {
	auto entry = std::find_if(cbegin(ObjectiveNames), cend(ObjectiveNames), [&](auto e) { return e.second == name; });
	if (entry == cend(ObjectiveNames)) {
		throw std::invalid_argument{"Unknown objective '" + name + "'"};
	}
	return entry->first;
}

auto isKnownObjective(int candidate) -> bool {
	return candidate >= static_cast<int>(ObjectiveId::None) && candidate < static_cast<int>(ObjectiveId::INVALID_ID);
}

} // namespace Kinova