#ifndef INCLUDE_CONTROL_OBJECTIVE_MANAGER_H_
#define INCLUDE_CONTROL_OBJECTIVE_MANAGER_H_

#include "control/Objective.h"
#include "support/Logging.h"

#include <map>
#include <vector>

namespace KinovaZED::Control {

struct ObjectiveManager : LoggingMixin {
	ObjectiveManager(std::istream &in, Logger logger);

	auto getObjective(Objective::Id id) const -> Objective const &;
	auto getObjective(Objective::Id id) -> Objective &;

  private:
	auto loadObjectives(std::istream &in) -> void;

	std::map<Objective::Id, Objective> objectives{};
};

} // namespace KinovaZED::Control

#endif
