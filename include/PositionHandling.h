#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include "Coordinates.h"
#include "Logging.h"
#include "Matrix.h"
#include "Objective.h"
#include "Sequence.h"

#include <map>
#include <set>
#include <vector>

struct PositionHandling {
	using f2d_vec_t = std::vector<std::vector<float>>;
	using f3d_vec_t = std::vector<f2d_vec_t>;

	PositionHandling(std::istream &in, Logging::Logger logger);
	PositionHandling(Logging::Logger logger);

	auto getObjective(Kinova::ObjectiveId id) const -> Kinova::Objective const &;
	auto getObjective(Kinova::ObjectiveId id) -> Kinova::Objective &;

	auto getCoordinates(Kinova::ObjectiveId id) const -> Kinova::Coordinates;

	auto setZeroObjective(Kinova::Coordinates coordinates, Kinova::ObjectiveId) -> void;

	auto incrementSequence(Kinova::ObjectiveId id) -> void;
	auto decrementSequence(Kinova::ObjectiveId id) -> void;
	auto resetSequence(Kinova::ObjectiveId id) -> void;

	auto savePoint(Kinova::Coordinates point, Kinova::ObjectiveId id) -> bool;
	auto saveOrigin(Kinova::Coordinates point, Kinova::ObjectiveId id) -> void;
	auto deletePoint(Kinova::ObjectiveId id) -> void;

	auto getSequence(Kinova::ObjectiveId id) const -> int;
	auto resetOriginAtEnd(Kinova::ObjectiveId id) -> bool;

  private:
	Logging::Logger logger;
	std::map<Kinova::ObjectiveId, Kinova::Objective> objectives{};
};

#endif
