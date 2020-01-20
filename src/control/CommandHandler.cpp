#include "control/CommandHandler.h"

#include "comm/CommandInterface.h"
#include "control/ObjectiveManager.h"
#include "hw/Actor.h"
#include "hw/Coordinates.h"

#include <sml/sml.hpp>

#include <cassert>
#include <memory>

namespace KinovaZED::Control {

CommandHandler::CommandHandler(Comm::CommandInterface &interface,
                               Hw::Actor &actor,
                               ObjectiveManager &objectiveManager,
                               Logger logger)
    : commandSource(interface)
    , arm{actor}
    , objectiveManager{objectiveManager}
    , logger{logger}
    , stateMachine{} {
}

auto CommandHandler::process(Comm::Command command) -> void {
	using namespace Comm;

	switch (command.id) {
	case Command::Id::EStop:
		logger->warn("CommandHandler::process: received emergency stop request.");
		stateMachine.process_event(CoreStateMachine::Event::EStop{arm});
		break;
	case Command::Id::GoToPosition: {
		auto objectiveName = std::any_cast<std::string>(command.parameters[0]);
		assert(isKnownObjectiveId(objectiveName));
		currentObjective = std::ref(objectiveManager.getObjective(fromString<Objective::Id>(objectiveName)));
		logger->info("CommandHandler::process: moving toward objective '{0}'", objectiveName);
		auto point = currentObjective->nextPoint();
		stateMachine.process_event(CoreStateMachine::Event::GoToPosition{arm, *point});
	} break;
	case Command::Id::Initialize:
		logger->info("CommandHandler::process: initializing arm");
		stateMachine.process_event(CoreStateMachine::Event::Initialize{arm});
		logger->info("CommandHandler::process: arm was initialized");
		stateMachine.process_event(CoreStateMachine::Event::Retract{arm});
		break;
	case Command::Id::Unfold:
		logger->info("CommandHandler::process: unfolding the arm");
		stateMachine.process_event(CoreStateMachine::Event::Unfold{arm});
		break;
	default:
		logger->warn("CommandHandler::process: ignoring command '{0}'", toString(command.id));
	}
}

auto CommandHandler::onPositionReached(Hw::Actor &who, Hw::Coordinates point) -> void {
	using namespace boost::sml;

	logger->debug("CommandHandler::onPositionReached: the arm reached a trajectory point.");

	if (stateMachine.is("runningSequence"_s)) {
		assert(currentObjective);
		auto nextPoint = currentObjective->nextPoint();
		if (nextPoint) {
			logger->debug("CommandHandler::onPositionReached: moving towards a trajectory point.");
			stateMachine.process_event(CoreStateMachine::Event::GoToPosition{arm, *nextPoint});
		} else {
			logger->debug("CommandHandler::onPositionReached: trajectory was finished");
			stateMachine.process_event(CoreStateMachine::Event::SequenceFinished{});
		}
	}
	(void)who, (void)point;
}

auto CommandHandler::onHomeReached(Hw::Actor &who) -> void {
	stateMachine.process_event(CoreStateMachine::Event::Unfolded{});
	(void)who;
}

auto CommandHandler::onRetractionPointReached(Hw::Actor &who) -> void {
	(void)who;
	stateMachine.process_event(CoreStateMachine::Event::Retracted{});
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

auto makeCommandHandler(Comm::CommandInterface &interface,
                        Hw::Actor &actor,
                        ObjectiveManager &objectiveManager,
                        Logger logger) -> std::shared_ptr<CommandHandler> {
	auto handler = std::make_shared<CommandHandlerCtorAccess>(interface, actor, objectiveManager, logger);
	handler->enableSubscriptions();
	return handler;
}


} // namespace KinovaZED::Control