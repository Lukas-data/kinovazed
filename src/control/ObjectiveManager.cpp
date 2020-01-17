#include "control/ObjectiveManager.h"

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

ObjectiveManager::ObjectiveManager(Logger logger)
    : logger{logger} {
}

ObjectiveManager::ObjectiveManager(std::istream &in, Logger logger)
    : ObjectiveManager{logger} {
	if (in.peek() != std::remove_reference_t<decltype(in)>::traits_type::eof()) {
		for (auto &objective : loadObjectives(in, logger)) {
			objectives.emplace(objective.getId(), objective);
		}
	}
	objectives.emplace(Objective::Id::None, Objective{logger});
}

auto ObjectiveManager::getObjective(Objective::Id id) const -> Objective const & {
	return objectives.at(id);
}

auto ObjectiveManager::getObjective(Objective::Id id) -> Objective & {
	return objectives.at(id);
}

auto ObjectiveManager::getCoordinates(Objective::Id id) const -> Hw::Coordinates {
	if (id == Objective::Id::None) {
		throw std::invalid_argument{"Cannot call GetCoordinates() for objective 'None'"};
	}

	auto const &objective = getObjective(id);
	if (!objective.numberOfSequencePoints()) {
		throw std::invalid_argument{"Objective '" + toString(id) + "' does not have a sequence!"};
	}

	auto const &origin = objective.getOrigin();
	auto const &currentPoint = objective.getCurrentSequencePoint();
	auto const coordinates = currentPoint.isZero() ? origin.getCoordinates() : objective.getTransformedSequencePoint();

	logger->debug("ObjectiveManager::getCoordinates: TargetCoordinates: {0}", coordinates);
	return coordinates;
}

auto ObjectiveManager::setZeroObjective(Hw::Coordinates coordinates, Objective::Id id) -> void {
	auto &objective = objectives.at(id);
	if (!objective.isAbsolute() && !objective.currentSequenceIndex()) {
		objective.setOrigin(coordinates);
	}
}

auto ObjectiveManager::incrementSequence(Objective::Id targetObjective) -> void {
	objectives.at(targetObjective).forwardSequence();
}

auto ObjectiveManager::decrementSequence(Objective::Id targetObjective) -> void {
	objectives.at(targetObjective).rewindSequence();
}

auto ObjectiveManager::resetSequence(Objective::Id targetObjective) -> void {
	objectives.at(targetObjective).resetSequence();
}

auto ObjectiveManager::savePoint(Hw::Coordinates point, Objective::Id id) -> bool {
	auto &objective = objectives.at(id);
	auto const result = objective.saveSequencePoint(point);
	if (!result) {
		logger->warn("ObjectiveManager::SavePoint: SequenceCounter is out of bound: {0} at SequenceSize off {1}",
		             objective.currentSequenceIndex(),
		             objective.numberOfSequencePoints());
	}
	return result;
}

auto ObjectiveManager::saveOrigin(Hw::Coordinates point, Objective::Id id) -> void {
	objectives.at(id).setOrigin(point);
}

auto ObjectiveManager::deletePoint(Objective::Id id) -> void {
	if (id != Objective::Id::None) {
		objectives.at(id).deleteSequencePoint();
	}
}

auto ObjectiveManager::getSequence(Objective::Id id) const -> int {
	return static_cast<int>(objectives.at(id).currentSequenceIndex());
}

auto ObjectiveManager::resetOriginAtEnd(Objective::Id id) -> bool {
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