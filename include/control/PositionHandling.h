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

	auto getObjective(Objective::Id id) const -> Objective const &;
	auto getObjective(Objective::Id id) -> Objective &;

	auto getCoordinates(Objective::Id id) const -> Hw::Coordinates;

	auto setZeroObjective(Hw::Coordinates coordinates, Objective::Id) -> void;

	auto incrementSequence(Objective::Id id) -> void;
	auto decrementSequence(Objective::Id id) -> void;
	auto resetSequence(Objective::Id id) -> void;

	auto savePoint(Hw::Coordinates point, Objective::Id id) -> bool;
	auto saveOrigin(Hw::Coordinates point, Objective::Id id) -> void;
	auto deletePoint(Objective::Id id) -> void;

	auto getSequence(Objective::Id id) const -> int;
	auto resetOriginAtEnd(Objective::Id id) -> bool;

  private:
	Logger logger;
	std::map<Objective::Id, Objective> objectives{};
};

} // namespace KinovaZED::Control

#endif
