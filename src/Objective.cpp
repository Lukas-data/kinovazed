#include "Objective.h"

#include "Constants.h"
#include "Sequence.h"
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

Objective::Objective(Logging::Logger logger)
    : id{ObjectiveId::None}
    , origin{}
    , sequence{logger}
    , absolute{} {
}

Objective::Objective(nlohmann::json const &json, Logging::Logger logger)
    : id{getObjectiveId(json[KeyName].get<std::string>())}
    , origin{json[KeyOrigin].get<Coordinates>()}
    , sequence{json[KeySequence].get<std::vector<Coordinates>>(), logger}
    , absolute{json[KeyIsAbsolute].get<bool>()} {
	logger->info(
	    "Loaded objective '{0}: {1}'", static_cast<std::underlying_type_t<ObjectiveId>>(id), getObjectiveName(id));
}

auto Objective::getOrigin() const -> Origin {
	return origin;
}

auto Objective::setOrigin(Coordinates point) -> void {
	origin = Origin{point};
}

auto Objective::getCurrentSequencePoint() const -> Coordinates {
	return sequence.getCurrentCoordinates();
}

auto Objective::getTransformedSequencePoint() const -> Coordinates {
	return sequence.getTransformedCoordinates(origin);
}

auto Objective::saveSequencePoint(Coordinates point) -> bool {
	return sequence.savePoint(origin, point);
}

auto Objective::deleteSequencePoint() -> void {
	sequence.deletePoint();
}

auto Objective::numberOfSequencePoints() const -> std::size_t {
	return sequence.numberOfPoints();
}

auto Objective::currentSequenceIndex() const -> std::size_t {
	return sequence.currentSequencePoint();
}

auto Objective::forwardSequence() -> void {
	sequence.nextPoint();
}

auto Objective::rewindSequence() -> void {
	sequence.previousPoint();
}

auto Objective::resetSequence() -> void {
	sequence.reset();
}

auto Objective::sequenceEnded() const -> bool {
	return sequence.endReached();
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
	    {KeyOrigin, objective.origin.getCoordinates()},
	    {KeySequence, objective.sequence},
	};
}

} // namespace Kinova
