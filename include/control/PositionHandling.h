#ifndef INCLUDE_CONTROL_POSITION_HANDLING_H_
#define INCLUDE_CONTROL_POSITION_HANDLING_H_

#include "control/Objective.h"
#include "control/Sequence.h"
#include "hw/Coordinates.h"
#include "math/Matrix.h"
#include "support/Logging.h"

#include <map>
#include <set>
#include <vector>

namespace KinovaZED::Control {

struct PositionHandling {
	using f2d_vec_t = std::vector<std::vector<float>>;
	using f3d_vec_t = std::vector<f2d_vec_t>;

	PositionHandling(std::istream &in, Logger logger);
	PositionHandling(Logger logger);

	auto getObjective(ObjectiveId id) const -> Objective const &;
	auto getObjective(ObjectiveId id) -> Objective &;

	auto getCoordinates(ObjectiveId id) const -> Hw::Coordinates;

	auto setZeroObjective(Hw::Coordinates coordinates, ObjectiveId) -> void;

	auto incrementSequence(ObjectiveId id) -> void;
	auto decrementSequence(ObjectiveId id) -> void;
	auto resetSequence(ObjectiveId id) -> void;

	auto savePoint(Hw::Coordinates point, ObjectiveId id) -> bool;
	auto saveOrigin(Hw::Coordinates point, ObjectiveId id) -> void;
	auto deletePoint(ObjectiveId id) -> void;

	auto getSequence(ObjectiveId id) const -> int;
	auto resetOriginAtEnd(ObjectiveId id) -> bool;

  private:
	Logger logger;
	std::map<ObjectiveId, Objective> objectives{};
};

} // namespace KinovaZED::Control

#endif
