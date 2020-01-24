#include "control/CommandHandler.h"

#include "comm/CommandInterface.h"
#include "comm/Notification.h"
#include "control/ObjectiveManager.h"
#include "hw/Actor.h"
#include "hw/Coordinates.h"

#include <sml/sml.hpp>
#include <spdlog/fmt/fmt.h>

#include <bitset>
#include <cassert>
#include <functional>
#include <memory>
#include <tuple>

namespace KinovaZED::Control {

namespace {

auto extractJoystickPosition(Comm::Command command) -> std::tuple<int, int, int> {
	auto x = std::any_cast<int>(command.parameters[0]);
	auto y = std::any_cast<int>(command.parameters[1]);
	auto z = std::any_cast<int>(command.parameters[2]);
	return {x, y, z};
}

} // namespace

CommandHandler::CommandHandler(Comm::CommandInterface &interface,
                               Hw::Actor &actor,
                               ObjectiveManager &objectiveManager,
                               Logger logger)
    : LoggingMixin{logger, "CommandHandler"}
    , commandSource(interface)
    , arm{actor}
    , objectiveManager{objectiveManager}
    , stateMachine{CoreStateMachine{logger}} {
}

auto CommandHandler::process(Comm::Command command) -> void {
	using namespace Comm;
	using namespace std::placeholders;

	auto logStep = makeLoggedStepper("process");

	switch (command.id) {
	case Command::Id::EStop:
		logWarning("process", "received emergency stop request.");
		isInitialized = !logStep(CoreStateMachine::Event::EStop{arm},
		                         "entered emergency stop state",
		                         "internal state machine refused to enter emergency stop.");
		break;
	case Command::Id::QuitEStop:
		logStep(CoreStateMachine::Event::QuitEStop{arm},
		        "leaving emergency stop mode",
		        "internal state machine refused to leave the emergency stop mode");
		break;
	case Command::Id::GoToPosition: {
		currentObjective = objectiveManager.getObjective(std::any_cast<Objective::Id>(command.parameters[0]));
		auto point = currentObjective->nextPoint();
		auto name = toString(currentObjective->getId());
		logStep(CoreStateMachine::Event::GoToPosition{arm, *point},
		        fmt::format("moving toward objective '{}'", name),
		        fmt::format("internal state machine refused to move toward objective '{}'", name));
	} break;
	case Command::Id::GoToSafe: {
		currentObjective = objectiveManager.getObjective(std::any_cast<Objective::Id>(command.parameters[0]));
		auto point = currentObjective->nextPoint();
		auto name = toString(currentObjective->getId());
		logStep(CoreStateMachine::Event::GoToSafe{arm, *point},
		        fmt::format("moving toward safety objective '{}'", name),
		        fmt::format("internal state machine refused to move toward safety objective '{}'", name));
	} break;
	case Command::Id::Initialize:
		logStep(CoreStateMachine::Event::Initialize{arm},
		        "initializing arm",
		        "internal state machine refused to initialize the arm");
		break;
	case Command::Id::Unfold:
		logStep(CoreStateMachine::Event::Unfold{arm},
		        "unfolding the arm",
		        "internal state machine refused to unfold the arm");
		break;
	case Command::Id::SetMode: {
		auto mode = std::any_cast<Hw::SteeringMode>(command.parameters[0]);
		auto name = toString(mode);
		logStep(CoreStateMachine::Event::SetMode{arm, mode},
		        fmt::format("changing steering mode to '{}'", name),
		        fmt::format("internal state machine refused to change mode to '{}'", name));

	} break;
	case Command::Id::NoMode:
		logStep(CoreStateMachine::Event::SetMode{arm, Hw::SteeringMode::NoMode},
		        "changing into NoMode steering mode",
		        "internal state machine refused to change into NoMode mode");
		break;
	case Command::Id::MoveJoystick: {
		auto [x, y, z] = extractJoystickPosition(command);
		logStep(CoreStateMachine::Event::JoystickMoved{arm, x, y, z},
		        fmt::format("moving joystick to '[{}, {}, {}]'", x, y, z),
		        fmt::format("internal state machine refused to move joystick to '[{}, {}, {}]'", x, y, z));
	} break;
	default:
		logWarning("process", "ignoring command '{0}'", toString(command.id));
	}
}

auto CommandHandler::onPositionReached(Hw::Actor &, Hw::Coordinates) -> void {
	using namespace boost::sml;

	logDebug("onPositionReached", "the arm reached a trajectory point");

	auto logStep = makeLoggedStepper("onPositionReached");

	if (!(stateMachine.is(CoreStateMachine::runningSequence) ||
	      stateMachine.is(CoreStateMachine::runningSafeSequence))) {
		logWarning("onPositionReached", "reached a trajectory point without an active sequence");
		return;
	}

	assert(currentObjective);
	auto nextPoint = currentObjective->nextPoint();

	if (nextPoint) {
		if (stateMachine.is(CoreStateMachine::runningSequence)) {
			logStep(CoreStateMachine::Event::GoToPosition{arm, *nextPoint},
			        "moving towards next sequence point",
			        "internal state machine refused to move towards next sequence point");
		} else {
			logStep(CoreStateMachine::Event::GoToSafe{arm, *nextPoint},
			        "moving towards next safety sequence point",
			        "internal state machine refused to move towards next safety sequence point");
		}
	} else {
		if (logStep(CoreStateMachine::Event::SequenceFinished{arm},
		            "finishing movement sequence",
		            "internal state machine did not accept sequence end event")) {
			commandSource.send(Comm::Notification{Comm::Notification::Id::SequenceDone});
		}
	}
}

auto CommandHandler::onHomeReached(Hw::Actor &) -> void {
	auto logStep = makeLoggedStepper("onHomeReached");

	logStep(CoreStateMachine::Event::Unfolded{},
	        "marking the arm as being at home position",
	        "internal state machine did not accept unfolded event");
}

auto CommandHandler::onRetractionPointReached(Hw::Actor &) -> void {
	auto logStep = makeLoggedStepper("onRetractionPointReached");

	logStep(CoreStateMachine::Event::Retracted{},
	        "marking the arm as being in the retracted",
	        "internal state machine did not accept retracted event");
}

auto CommandHandler::onSteeringModeChanged(Hw::Actor &, Hw::SteeringMode mode) -> void {
	auto logStep = makeLoggedStepper("onSteeringModeChanged");

	logInfo("onSteeringModeChanged", "steering mode changed to '{}'", toString(mode));
	if (logStep(CoreStateMachine::Event::ModeSet{mode},
	            "changed steering mode",
	            "internal state machine did not accept steering mode change")) {
		commandSource.send(Comm::Notification{Comm::Notification::Id::ModeSet});
	}
}

auto CommandHandler::onReconnectedDueToError(Hw::Actor &) -> void {
}

auto CommandHandler::onInitializationFinished(Hw::Actor &) -> void {
	auto logStep = makeLoggedStepper("onInitializationFinished");

	if ((isInitialized = logStep(CoreStateMachine::Event::Initialized{},
	                             "marking the arm as initialized",
	                             "internal state machine did not accept initialized event"))) {
		commandSource.send(Comm::Notification{Comm::Notification::Id::Initialized});
	}
}

auto CommandHandler::getSystemState() -> std::bitset<8> {
	auto state = std::bitset<8>{};

	state.set(0, !arm.hasFailed());
	state.set(1, stateMachine.is(CoreStateMachine::emergencyStopped));
	state.set(2, isInitialized);
	state.set(3, stateMachine.is(CoreStateMachine::safe));

	return state;
}

struct CommandHandlerCtorAccess : CommandHandler {
	template<typename... Args>
	CommandHandlerCtorAccess(Args &&... args)
	    : CommandHandler(std::forward<Args>(args)...) {
	}

	auto enableSubscriptions() -> void {
		commandSource.subscribe(shared_from_this());
		logDebug("enableSubscriptions", "subscribed to control commands");
		arm.subscribe(shared_from_this());
		logDebug("enableSubscriptions", "subscribed to hardware events");
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