#ifndef INCLUDE_CONTROL_COMMAND_HANDLING_H_
#define INCLUDE_CONTROL_COMMAND_HANDLING_H_

#include "comm/CommandInterface.h"
#include "comm/CommandSubscriber.h"
#include "control/StateMachine.h"
#include "hw/Actor.h"
#include "support/Logging.h"

#include <memory>

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

  protected:
	CommandHandler(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger);

  private:
	Comm::CommandInterface &commandSource;
	Hw::Actor &arm;
	Logger logger;

	StateMachine stateMachine{logger};
};

auto makeCommandHandler(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger)
    -> std::shared_ptr<CommandHandler>;

} // namespace KinovaZED::Control
#endif
