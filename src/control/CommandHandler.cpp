#include "control/CommandHandler.h"

#include "comm/CommandInterface.h"
#include "control/ObjectiveManager.h"
#include "hw/Actor.h"
#include "hw/Coordinates.h"

#include <sml/sml.hpp>
#include <spdlog/fmt/fmt.h>

#include <cassert>
#include <functional>
#include <memory>

namespace KinovaZED::Control {

namespace {

auto extractObjective(Comm::Command command, ObjectiveManager const &manager) -> Objective {
	auto objectiveName = std::any_cast<std::string>(command.parameters[0]);
	assert(isKnownObjectiveId(objectiveName));
	auto objectiveId = fromString<Objective::Id>(objectiveName);
	return manager.getObjective(objectiveId);
}

} // namespace

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
	using namespace std::placeholders;

	auto logLocalStep = [this](auto event, auto success, auto failure) {
		return logStep(event, "process", success, failure);
	};

	switch (command.id) {
	case Command::Id::EStop:
		logger->warn("CommandHandler::process: received emergency stop request.");
		logLocalStep(CoreStateMachine::Event::EStop{arm},
		             "entered emergency stop state",
		             "internal state machine refused to enter emergency stop.");
		break;
	case Command::Id::GoToPosition: {
		currentObjective = extractObjective(command, objectiveManager);
		auto point = currentObjective->nextPoint();
		auto name = toString(currentObjective->getId());
		logLocalStep(CoreStateMachine::Event::GoToPosition{arm, *point},
		             fmt::format("moving toward objective '{}'", name),
		             fmt::format("internal state machine refused to move toward objective '{}'", name));
	} break;
	case Command::Id::Initialize:
		logLocalStep(CoreStateMachine::Event::Initialize{arm},
		             "initializing arm",
		             "internal state machine refused to initialize the arm");
		break;
	case Command::Id::Unfold:
		logLocalStep(CoreStateMachine::Event::Unfold{arm},
		             "unfolding the arm",
		             "internal state machine refused to unfold the arm");
		break;
	case Command::Id::QuitEStop:
		logLocalStep(CoreStateMachine::Event::QuitEStop{arm},
		             "leaving emergency stop mode",
		             "internal state machine refused to leave the emergency stop mode");
		break;
	default:
		logger->warn("CommandHandler::process: ignoring command '{0}'", toString(command.id));
	}
}

auto CommandHandler::onPositionReached(Hw::Actor &, Hw::Coordinates) -> void {
	using namespace boost::sml;

	logger->debug("CommandHandler::onPositionReached: the arm reached a trajectory point.");

	auto logLocalStep = [this](auto event, auto success, auto failure) {
		return logStep(event, "onPositionReached", success, failure);
	};

	if (stateMachine.is("runningSequence"_s)) {
		assert(currentObjective);
		auto nextPoint = currentObjective->nextPoint();
		if (nextPoint) {
			logLocalStep(CoreStateMachine::Event::GoToPosition{arm, *nextPoint},
			             "moving towards next sequence point",
			             "internal state machine refused to move towards next sequence point");
		} else {
			logLocalStep(CoreStateMachine::Event::SequenceFinished{},
			             "finishing movement sequence",
			             "internal state machine did not accept sequence end event");
		}
	}
}

auto CommandHandler::onHomeReached(Hw::Actor &) -> void {
	auto logLocalStep = [this](auto event, auto success, auto failure) {
		return logStep(event, "onHomeReached", success, failure);
	};

	logLocalStep(CoreStateMachine::Event::Unfolded{},
	             "marking the arm as being at home position",
	             "internal state machine did not accept unfolded event");
}

auto CommandHandler::onRetractionPointReached(Hw::Actor &) -> void {
	auto logLocalStep = [this](auto event, auto success, auto failure) {
		return logStep(event, "onHomeReached", success, failure);
	};

	logLocalStep(CoreStateMachine::Event::Retracted{},
	             "marking the arm as being in the retracted",
	             "internal state machine did not accept retracted event");
}

auto CommandHandler::onSteeringModeChanged(Hw::Actor &, Hw::SteeringMode) -> void {
}

auto CommandHandler::onReconnectedDueToError(Hw::Actor &) -> void {
}

auto CommandHandler::onInitializationFinished(Hw::Actor &) -> void {
	auto logLocalStep = [this](auto event, auto success, auto failure) {
		return logStep(event, "onInitializationFinished", success, failure);
	};

	auto acceptedInitialized = logLocalStep(CoreStateMachine::Event::Initialized{},
	                                        "marking the arm as initialized",
	                                        "internal state machine did not accept initialized event");

	if (acceptedInitialized) {
		acceptedInitialized = logLocalStep(CoreStateMachine::Event::Retract{arm},
		                                   "retracting the arm",
		                                   "internal state machine refused to retract the arm");
	}
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