#include "control/Objective.h"

#include "math/Matrix.h"
#include "support/Constants.h"
#include "support/EnumUtils.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace KinovaZED::Control {

auto constexpr objectiveNames = std::array{
    // clang-format off

	std::pair{Objective::Id::Antenna, "Antenna"},
	std::pair{Objective::Id::AntennaPull, "AntennaPull"},
	std::pair{Objective::Id::Bell, "Bell"},
	std::pair{Objective::Id::Handle, "Handle"},
	std::pair{Objective::Id::OpenDoor, "OpenDoor"},
	std::pair{Objective::Id::PlaceCup, "PlaceCup"},
	std::pair{Objective::Id::PullDoor, "PullDoor"},

    // clang-format on
};

static_assert(enumNameMappingsAreUnique(objectiveNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(objectiveNames, Objective::Id::Bell), "Missing entry in name map!");

auto isKnownObjectiveId(int candidate) -> bool {
	return candidate >= 0 && candidate < static_cast<int>(Objective::Id::END_OF_ENUM);
}

auto isKnownObjectiveId(std::string const &candidate) -> bool {
	auto found = std::find_if(
	    cbegin(objectiveNames), cend(objectiveNames), [&](auto entry) { return entry.second == candidate; });
	return found != cend(objectiveNames);
}

auto constexpr KeyIsAbsolute = "is_abs";
auto constexpr KeyName = "name";
auto constexpr KeyOrigin = "origin";
auto constexpr KeySequence = "sequence";

Objective::Objective(nlohmann::json const &json, Logger logger)
    : LoggingMixin{logger, "Objective"}
    , id{fromString<Id>(json[KeyName].get<std::string>())}
    , origin{json[KeyOrigin].get<Hw::Coordinates>()}
    , sequence{json[KeySequence].get<std::vector<Hw::Coordinates>>()}
    , absolute{json[KeyIsAbsolute].get<bool>()} {
	logInfo("<ctor>", "loaded objective '{0}'", toString(id));
}

auto Objective::getOrigin() const -> Hw::Origin {
	return origin;
}

auto Objective::setOrigin(Hw::Coordinates point) -> void {
	origin = Hw::Origin{point};
}

auto Objective::nextPoint() -> std::optional<Hw::Coordinates> {
	++currentSequenceIndex;
	if (currentSequenceIndex < static_cast<decltype(currentSequenceIndex)>(sequence.size())) {
		auto coordinates = sequence[currentSequenceIndex];
		return coordTransform(coordinates, origin.getTransformationMatrix());
	}
	return std::nullopt;
}

auto Objective::getId() const -> Id {
	return id;
}

auto Objective::isAbsolute() const -> bool {
	return absolute;
}

auto to_json(nlohmann::json &output, Objective const &objective) -> void {
	output = nlohmann::json{
	    {KeyIsAbsolute, objective.absolute},
	    {KeyName, toString(objective.id)},
	    {KeyOrigin, objective.isAbsolute() ? objective.origin.getCoordinates() : Hw::Coordinates{}},
	    {KeySequence, objective.sequence},
	};
}

} // namespace KinovaZED::Control

// @section ToString support

namespace KinovaZED {

using namespace Control;

template<>
auto toString(Objective::Id const &id) -> std::string {
	assert(isKnownObjectiveId(static_cast<std::underlying_type_t<Objective::Id>>(id)));

	auto found =
	    std::find_if(cbegin(objectiveNames), cend(objectiveNames), [&](auto entry) { return entry.first == id; });
	return found->second;
}

template<>
auto fromString(std::string const &name) -> Objective::Id {
	auto found =
	    std::find_if(cbegin(objectiveNames), cend(objectiveNames), [&](auto entry) { return entry.second == name; });

	assert(found != cend(objectiveNames));

	return found->first;
}

} // namespace KinovaZED