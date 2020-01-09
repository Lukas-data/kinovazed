#include "PositionHandling.h"

#include "Constants.h"
#include "Exceptions.h"
#include "Logging.h"
#include "Paths.h"
#include "Sequence.h"
#include "nlohmann/json.hpp"
#include "spdlog/fmt/ostr.h"

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

PositionHandling::PositionHandling(Logging::Logger logger)
    : logger{logger} {
}

PositionHandling::PositionHandling(std::istream &in, Logging::Logger logger)
    : PositionHandling{logger} {
	if (in.peek() != std::remove_reference_t<decltype(in)>::traits_type::eof()) {
		for (auto &objective : Kinova::loadObjectives(in, logger)) {
			objectives.emplace(objective.getId(), objective);
		}
	}
	objectives.emplace(Kinova::ObjectiveId::None, Kinova::Objective{logger});
}

auto PositionHandling::getObjective(Kinova::ObjectiveId id) const -> Kinova::Objective const & {
	return objectives.at(id);
}

auto PositionHandling::getObjective(Kinova::ObjectiveId id) -> Kinova::Objective & {
	return objectives.at(id);
}

auto PositionHandling::getCoordinates(Kinova::ObjectiveId id) const -> Kinova::Coordinates {
	if (id == Kinova::ObjectiveId::None) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for objective 'None'"};
	}

	auto const &objective = getObjective(id);
	if (!objective.numberOfSequencePoints()) {
		throw std::invalid_argument{"Objective '" + Kinova::getObjectiveName(id) + "' does not have a sequence!"};
	}

	auto const &origin = objective.getOrigin();
	auto const &currentPoint = objective.getCurrentSequencePoint();
	auto const coordinates = currentPoint.isZero() ? origin.getCoordinates() : objective.getTransformedSequencePoint();

	logger->debug("PositionHandling::getCoordinates: TargetCoordinates: {0}", coordinates);
	return coordinates;
}

auto PositionHandling::setZeroObjective(Kinova::Coordinates coordinates, Kinova::ObjectiveId id) -> void {
	auto &objective = objectives.at(id);
	if (!objective.isAbsolute() && !objective.currentSequenceIndex()) {
		objective.setOrigin(coordinates);
	}
}

auto PositionHandling::incrementSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).forwardSequence();
}

auto PositionHandling::decrementSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).rewindSequence();
}

auto PositionHandling::resetSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).resetSequence();
}

auto PositionHandling::savePoint(Kinova::Coordinates point, Kinova::ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	auto const result = objective.saveSequencePoint(point);
	if (!result) {
		logger->warn("PositionHandling::SavePoint: SequenceCounter is out of bound: {0} at SequenceSize off {1}",
		             objective.currentSequenceIndex(),
		             objective.numberOfSequencePoints());
	}
	return result;
}

auto PositionHandling::saveOrigin(Kinova::Coordinates point, Kinova::ObjectiveId id) -> void {
	objectives.at(id).setOrigin(point);
}

auto PositionHandling::deletePoint(Kinova::ObjectiveId id) -> void {
	if (id != Kinova::ObjectiveId::None) {
		objectives.at(id).deleteSequencePoint();
	}
}

auto PositionHandling::getSequence(Kinova::ObjectiveId id) const -> int {
	return static_cast<int>(objectives.at(id).currentSequenceIndex());
}

auto PositionHandling::resetOriginAtEnd(Kinova::ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	if (!objective.sequenceEnded()) {
		return false;
	}
	if (!objective.isAbsolute()) {
		objective.setOrigin(Kinova::Coordinates{});
	}
	return true;
}
