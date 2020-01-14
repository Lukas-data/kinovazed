#include "control/Objective.h"

#include "control/Sequence.h"
#include "support/Constants.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace KinovaZED::Control {

auto constexpr KeyIsAbsolute = "is_abs";
auto constexpr KeyName = "name";
auto constexpr KeyOrigin = "origin";
auto constexpr KeySequence = "sequence";

Objective::Objective(Logger logger)
    : id{ObjectiveId::None}
    , origin{}
    , sequence{logger}
    , absolute{} {
}

Objective::Objective(nlohmann::json const &json, Logger logger)
    : id{getObjectiveId(json[KeyName].get<std::string>())}
    , origin{json[KeyOrigin].get<Hw::Coordinates>()}
    , sequence{json[KeySequence].get<std::vector<Hw::Coordinates>>(), logger}
    , absolute{json[KeyIsAbsolute].get<bool>()} {
	logger->info(
	    "Loaded objective '{0}: {1}'", static_cast<std::underlying_type_t<ObjectiveId>>(id), getObjectiveName(id));
}

auto Objective::getOrigin() const -> Hw::Origin {
	return origin;
}

auto Objective::setOrigin(Hw::Coordinates point) -> void {
	origin = Hw::Origin{point};
}

auto Objective::getCurrentSequencePoint() const -> Hw::Coordinates {
	return sequence.currentPoint();
}

auto Objective::getTransformedSequencePoint() const -> Hw::Coordinates {
	return sequence.currentPoint(origin);
}

auto Objective::saveSequencePoint(Hw::Coordinates point) -> bool {
	return sequence.putPoint(origin, point);
}

auto Objective::deleteSequencePoint() -> void {
	sequence.removeCurrentPoint();
}

auto Objective::numberOfSequencePoints() const -> std::size_t {
	return sequence.length();
}

auto Objective::currentSequenceIndex() const -> std::size_t {
	return sequence.currentStep();
}

auto Objective::forwardSequence() -> void {
	sequence.advance();
}

auto Objective::rewindSequence() -> void {
	sequence.stepBack();
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

auto loadObjectives(std::istream &in, Logger logger) -> std::vector<Objective> {
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

} // namespace KinovaZED::Control
