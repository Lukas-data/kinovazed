#ifndef INCLUDE_CONTROL_COMMAND_HANDLING_H_
#define INCLUDE_CONTROL_COMMAND_HANDLING_H_

#include "comm/CommandInterface.h"
#include "comm/CommandSubscriber.h"
#include "control/CoreStateMachine.h"
#include "control/Objective.h"
#include "control/ObjectiveManager.h"
#include "hw/Actor.h"
#include "support/Logging.h"

#include <sml/sml.hpp>

#include <memory>
#include <optional>

constexpr int numberOfJoystickMoveInputs = 3;

namespace KinovaZED::Control {

struct CommandHandler : std::enable_shared_from_this<CommandHandler>,
                        Comm::CommandSubscriber,
                        Hw::Actor::EventSubscriber {

	auto process(Comm::Command command) -> void;

	auto onPositionReached(Hw::Actor &who, Hw::Coordinates point) -> void;
	auto onHomeReached(Hw::Actor &who) -> void;
	auto onRetractionPointReached(Hw::Actor &who) -> void;
	auto onSteeringModeChanged(Hw::Actor &who, Hw::SteeringMode mode) -> void;
	auto onReconnectedDueToError(Hw::Actor &who) -> void;
	auto onInitializationFinished(Hw::Actor &who) -> void;

  protected:
	CommandHandler(Comm::CommandInterface &interface,
	               Hw::Actor &actor,
	               ObjectiveManager &objectiveManager,
	               Logger logger);

	Comm::CommandInterface &commandSource;
	Hw::Actor &arm;
	ObjectiveManager &objectiveManager;
	Logger logger;
	boost::sml::sm<CoreStateMachine> stateMachine;

	std::optional<Objective> currentObjective{};
};

auto makeCommandHandler(Comm::CommandInterface &interface,
                        Hw::Actor &actor,
                        ObjectiveManager &objectiveManager,
                        Logger logger) -> std::shared_ptr<CommandHandler>;

} // namespace KinovaZED::Control
#endif
