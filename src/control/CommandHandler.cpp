#include "control/CommandHandler.h"

#include "comm/CommandInterface.h"
#include "hw/Actor.h"
#include "hw/Coordinates.h"

#include <memory>

namespace KinovaZED::Control {

CommandHandler::CommandHandler(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger)
    : commandSource(interface)
    , arm{actor}
    , logger{logger} {
}

auto CommandHandler::process(Comm::Command command) -> void {
	switch (command.id) {
	case Comm::Command::Id::EStop:
		logger->warn("CommandHandler::process: received emergency stop request.");
		break;
	default:
		logger->info("CommandHandler::process: ignoring command '{0}'", toString(command.id));
	}
}

auto CommandHandler::onPositionReached(Hw::Actor &who, Hw::Coordinates point) -> void {
	(void)who, (void)point;
}

auto CommandHandler::onHomeReached(Hw::Actor &who) -> void {
	(void)who;
}

auto CommandHandler::onRetractionPointReached(Hw::Actor &who) -> void {
	(void)who;
}

auto CommandHandler::onSteeringModeChanged(Hw::Actor &who, Hw::SteeringMode mode) -> void {
	(void)who, (void)mode;
}

auto CommandHandler::onReconnectedDueToError(Hw::Actor &who) -> void {
	(void)who;
}

struct CommandHandlerCtorAccess : CommandHandler {
	template<typename... Args>
	CommandHandlerCtorAccess(Args &&... args)
	    : CommandHandler(std::forward<Args>(args)...) {
	}

	auto enableSubscriptions() -> void {
		commandSource.subscribe(shared_from_this());
		logger->debug("CommandHandler: subscribed to control commands");
		arm.subscribe(shared_from_this());
		logger->debug("CommandHandler: subscribed to hardware events");
	}
};

auto makeCommandHandler(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger)
    -> std::shared_ptr<CommandHandler> {
	auto handler = std::make_shared<CommandHandlerCtorAccess>(interface, actor, logger);
	handler->enableSubscriptions();
	return handler;
}


} // namespace KinovaZED::Control