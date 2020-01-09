#include "Objective.h"

#include "Constants.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace Kinova {

auto constexpr KeyIsAbsolute = "is_abs";
auto constexpr KeyName = "name";
auto constexpr KeyOrigin = "origin";
auto constexpr KeySequence = "sequence";

Objective::Objective(nlohmann::json const &json, Logging::Logger logger)
    : id{getObjectiveId(json[KeyName].get<std::string>())}
    , origin{json[KeyOrigin].get<Coordinates>()}
    , sequence{origin, json[KeySequence].get<std::vector<Coordinates>>(), logger}
    , absolute{json[KeyIsAbsolute].get<bool>()} {
	logger->info(
	    "Loaded objective '{0}: {1}'", static_cast<std::underlying_type_t<ObjectiveId>>(id), getObjectiveName(id));
}

auto Objective::getSequence() -> Sequence & {
	return sequence;
}

auto Objective::getSequence() const -> Sequence const & {
	return sequence;
}

auto Objective::hasOrigin() const -> bool {
	return !origin.isZero();
}

auto Objective::getOrigin() const -> Coordinates {
	return origin;
}

auto Objective::isAbsolute() const -> bool {
	return absolute;
}

auto Objective::getId() const -> ObjectiveId {
	return id;
}

auto loadObjectives(std::istream &in, Logging::Logger logger) -> std::vector<Kinova::Objective> {
	auto json = nlohmann::json::parse(in);

	if (!json.empty() && !json.is_array()) {
		logger->warn("Expected an array as the rool element!");
		return {};
	}

	auto objectives = std::vector<Objective>{};
	objectives.reserve(json.size());

	transform(cbegin(json), cend(json), back_inserter(objectives), [&](auto const &element) {
		return Objective{element, logger};
	});

	return objectives;
}

auto to_json(nlohmann::json &output, Objective const &objective) -> void {
	output = nlohmann::json{
	    {KeyIsAbsolute, objective.absolute},
	    {KeyName, getObjectiveName(objective.id)},
	    {KeyOrigin, objective.origin},
	    {KeySequence, objective.sequence.getPoints()},
	};
}

} // namespace Kinova
