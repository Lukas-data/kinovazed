#include "control/PositionHandling.h"

#include "control/Sequence.h"
#include "support/Constants.h"
#include "support/Logging.h"
#include "support/Paths.h"

#include <nlohmann/json.hpp>
#include <spdlog/fmt/ostr.h>

#include <algorithm>
#include <array>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace KinovaZED::Control {

PositionHandling::PositionHandling(Logger logger)
    : logger{logger} {
}

PositionHandling::PositionHandling(std::istream &in, Logger logger)
    : PositionHandling{logger} {
	if (in.peek() != std::remove_reference_t<decltype(in)>::traits_type::eof()) {
		for (auto &objective : loadObjectives(in, logger)) {
			objectives.emplace(objective.getId(), objective);
		}
	}
	objectives.emplace(ObjectiveId::None, Objective{logger});
}

auto PositionHandling::getObjective(ObjectiveId id) const -> Objective const & {
	return objectives.at(id);
}

auto PositionHandling::getObjective(ObjectiveId id) -> Objective & {
	return objectives.at(id);
}

auto PositionHandling::getCoordinates(ObjectiveId id) const -> Hw::Coordinates {
	if (id == ObjectiveId::None) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for objective 'None'"};
	}

	auto const &objective = getObjective(id);
	if (!objective.numberOfSequencePoints()) {
		throw std::invalid_argument{"Objective '" + getObjectiveName(id) + "' does not have a sequence!"};
	}

	auto const &origin = objective.getOrigin();
	auto const &currentPoint = objective.getCurrentSequencePoint();
	auto const coordinates = currentPoint.isZero() ? origin.getCoordinates() : objective.getTransformedSequencePoint();

	logger->debug("PositionHandling::getCoordinates: TargetCoordinates: {0}", coordinates);
	return coordinates;
}

auto PositionHandling::setZeroObjective(Hw::Coordinates coordinates, ObjectiveId id) -> void {
	auto &objective = objectives.at(id);
	if (!objective.isAbsolute() && !objective.currentSequenceIndex()) {
		objective.setOrigin(coordinates);
	}
}

auto PositionHandling::incrementSequence(ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).forwardSequence();
}

auto PositionHandling::decrementSequence(ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).rewindSequence();
}

auto PositionHandling::resetSequence(ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).resetSequence();
}

auto PositionHandling::savePoint(Hw::Coordinates point, ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	auto const result = objective.saveSequencePoint(point);
	if (!result) {
		logger->warn("PositionHandling::SavePoint: SequenceCounter is out of bound: {0} at SequenceSize off {1}",
		             objective.currentSequenceIndex(),
		             objective.numberOfSequencePoints());
	}
	return result;
}

auto PositionHandling::saveOrigin(Hw::Coordinates point, ObjectiveId id) -> void {
	objectives.at(id).setOrigin(point);
}

auto PositionHandling::deletePoint(ObjectiveId id) -> void {
	if (id != ObjectiveId::None) {
		objectives.at(id).deleteSequencePoint();
	}
}

auto PositionHandling::getSequence(ObjectiveId id) const -> int {
	return static_cast<int>(objectives.at(id).currentSequenceIndex());
}

auto PositionHandling::resetOriginAtEnd(ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	if (!objective.sequenceEnded()) {
		return false;
	}
	if (!objective.isAbsolute()) {
		objective.setOrigin(Hw::Coordinates{});
	}
	return true;
}

} // namespace KinovaZED::Control