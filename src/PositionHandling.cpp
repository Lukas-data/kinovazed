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
	if (objective.getSequence().getPoints().empty()) {
		throw std::invalid_argument{"Objective '" + Kinova::getObjectiveName(id) + "' does not have a sequence!"};
	}

	auto const &sequence = objective.getSequence();
	auto const coordinates =
	    sequence.getCurrentCoordinates().isZero() ? sequence.getOrigin() : sequence.getTransformedCoordinates();

	logger->debug("PositionHandling::getCoordinates: TargetCoordinates: {0}", coordinates);
	return coordinates;
}

auto PositionHandling::setZeroObjective(Kinova::Coordinates coordinates, Kinova::ObjectiveId id) -> void {
	auto &objective = objectives.at(id);
	if (!objective.isAbsolute() && !objective.getSequence().currentSequencePoint()) {
		objective.getSequence().setOrigin(coordinates);
	}
}

auto PositionHandling::incrementSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).getSequence().nextPoint();
}

auto PositionHandling::decrementSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).getSequence().previousPoint();
}

auto PositionHandling::resetSequence(Kinova::ObjectiveId targetObjective) -> void {
	objectives.at(targetObjective).getSequence().reset();
}

auto PositionHandling::savePoint(Kinova::Coordinates point, Kinova::ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	auto const result = objective.getSequence().savePoint(point);
	if (!result) {
		logger->warn("PositionHandling::SavePoint: SequenceCounter is out of bound: {0} at SequenceSize off {1}",
		             objective.getSequence().currentSequencePoint(),
		             objective.getSequence().numberOfPoints());
	}
	return result;
}

auto PositionHandling::saveOrigin(Kinova::Coordinates point, Kinova::ObjectiveId id) -> void {
	objectives.at(id).getSequence().setOrigin(point);
}

auto PositionHandling::deletePoint(Kinova::ObjectiveId id) -> void {
	if (id != Kinova::ObjectiveId::None) {
		objectives.at(id).getSequence().deletePoint();
	}
}

auto PositionHandling::getSequence(Kinova::ObjectiveId id) const -> int {
	return static_cast<int>(objectives.at(id).getSequence().currentSequencePoint());
}

auto PositionHandling::resetOriginAtEnd(Kinova::ObjectiveId id) -> bool {
	auto &objective = objectives.at(id);
	if (!objective.getSequence().endReached()) {
		return false;
	}
	if (!objective.isAbsolute()) {
		objective.getSequence().resetOrigin();
	}
	return true;
}
