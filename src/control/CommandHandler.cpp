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
	interface.subscribe(shared_from_this());
	actor.subscribe(shared_from_this());
}

auto CommandHandler::process(Comm::Command command) -> void {
	switch (command.id) {
	case Comm::Command::Id::E_Stop:
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

auto makeCommandHandler(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger)
    -> std::shared_ptr<CommandHandler> {
	struct CommandHandlerKey : CommandHandler {
		CommandHandlerKey(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger)
		    : CommandHandler(interface, actor, logger) {
		}
	};

	return std::make_shared<CommandHandlerKey>(interface, actor, logger);
}


} // namespace KinovaZED::Control


// #ifndef INCLUDE_CONTROL_COMMAND_HANDLING_H_
// #define INCLUDE_CONTROL_COMMAND_HANDLING_H_

// #include "comm/CommandInterface.h"
// #include "control/StateMachine.h"
// #include "hw/KinovaArm.h"
// #include "hw/KinovaStatus.h"
// #include "support/Logging.h"

// #include <chrono>
// #include <memory>
// #include <stdexcept>
// #include <thread>
// #include <utility>

// constexpr int numberOfJoystickMoveInputs = 3;

// namespace KinovaZED::Control {

// struct CommandHandling {
// 	CommandHandling(Comm::CommandInterface &interface, std::shared_ptr<Hw::KinovaArm> jacoZed, Logger logger)
// 	    : roboRio{std::move(roboRio)}
// 	    , jacoZed{jacoZed}
// 	    , logger{logger}
// 	    , kinovaSM{jacoZed, logger} {
// 		logger->debug("Connecting to RoboRio.");
// 		connectRoboRio();
// 		logger->debug("Connecting to JacoArm.");
// 		connectJacoZed();
// 	}

// 	CommandHandling(Logging::Logger logger)
// 	    : CommandHandling(std::make_unique<EventIo>(logger), std::make_shared<Arm>(logger), logger) {
// 	}

// 	void process() {
// 		Command newInCommand{KinovaFSM::NoEvent};
// 		bool processed = processInput(newInCommand);
// 		// OUTPUTS
// 		processOutput(processed, newInCommand, commandOut);
// 	}

//   private:
// 	std::unique_ptr<EventIo> roboRio;
// 	std::shared_ptr<Arm> jacoZed;
// 	Logging::Logger logger;
// 	StateMachine kinovaSM;

// 	Command commandOut{KinovaFSM::NoEvent};
// 	Command commandIn{KinovaFSM::NoEvent};

// 	/*Recieves Inputs from TCP Connection with RoboRIO connection. Writes Joystick Data directly to hardware.*/
// 	auto getInputs() -> Command {
// 		auto packet = roboRio->readPacket();
// 		Command commandIn = Command{static_cast<KinovaFSM::Event>(packet.command), packet.var};
// 		// TODO: (tcorbat) Shouldn't we check where whether event is actually MoveJoystick?
// 		jacoZed->setJoystick(packet.x, packet.y, packet.z);

// 		// Event is MoveJoystick, if Joystick moves and No Event is set.
// 		bool jSisZero = packet.x == 0 && packet.y == 0 && packet.z == 0;
// 		if (!jSisZero && commandIn.event != KinovaFSM::E_Stop) {
// 			commandIn = Command{KinovaFSM::MoveJoystick};
// 			logger->info("CommandHandling::getInputs(): MoveJoystick");
// 		}
// 		return commandIn;
// 	}

// 	void sendOutputs(Command const &command) {
// 		RoboRioProtocol::Packet packet{command.event, command.var, 0, 0, 0};
// 		roboRio->sendTCP(packet);
// 	}

// 	/*define Exception for Events, that should or should not be sent to Statemachine*/
// 	void checkInputEvent(Command &command) {
// 		// 'SetMode' is only sent when mode is not already set.
// 		int currentMode = jacoZed->getMode();
// 		if (jacoZed->getActive() && command == Command{KinovaFSM::SetMode, currentMode}) {
// 			command = Command{KinovaFSM::NoEvent};
// 		}

// 		//'GoToPosition' is only sent when the Arm is not already at the Position
// 		int currentPosition = jacoZed->getCurrentPosition();
// 		if (jacoZed->getActive() && command == Command{KinovaFSM::GoToPosition, currentPosition}) {
// 			command = Command{KinovaFSM::NoEvent};
// 		}
// 	}

// 	/*retrieves EventVariable for different HW-Events.*/
// 	auto getHWEventVar(KinovaFSM::Event const &event) -> int {
// 		// Hardware Events
// 		switch (event) {
// 		case KinovaFSM::ModeSet:
// 			return jacoZed->getMode();
// 		case KinovaFSM::SequenceDone:
// 			return jacoZed->getCurrentPosition();
// 		case KinovaFSM::PointReached:
// 		case KinovaFSM::PointSaved:
// 		case KinovaFSM::PointNotSaved:
// 		case KinovaFSM::PointDeleted:
// 		case KinovaFSM::PointNotDeleted:
// 		case KinovaFSM::PreviousPointNotSet:
// 		case KinovaFSM::PreviousPointSet:
// 		case KinovaFSM::NextPointNotSet:
// 		case KinovaFSM::NextPointSet:
// 			return jacoZed->getCurrentPoint();
// 		case KinovaFSM::Tick:
// 		case KinovaFSM::Initialize:
// 		case KinovaFSM::NoMode:
// 		case KinovaFSM::Retract:
// 		case KinovaFSM::Unfold:
// 		case KinovaFSM::SetMode:
// 		case KinovaFSM::MoveJoystick:
// 		case KinovaFSM::GoToPosition:
// 		case KinovaFSM::Teach:
// 		case KinovaFSM::SavePoint:
// 		case KinovaFSM::SaveOrigin:
// 		case KinovaFSM::DeletePoint:
// 		case KinovaFSM::Previous:
// 		case KinovaFSM::Next:
// 		case KinovaFSM::MoveOrigin:
// 		case KinovaFSM::Exit:
// 		case KinovaFSM::Shutdown:
// 		case KinovaFSM::E_Stop:
// 		case KinovaFSM::QuitEStop:
// 		case KinovaFSM::Initialized:
// 		case KinovaFSM::Retracted:
// 		case KinovaFSM::Unfolded:
// 		case KinovaFSM::PointNotReached:
// 		case KinovaFSM::OriginSaved:
// 		case KinovaFSM::OriginNotSaved:
// 		case KinovaFSM::InitHomeReached:
// 		case KinovaFSM::Error:
// 		default:
// 			return 0;
// 		}
// 	}

// 	void connectRoboRio() {
// 		using namespace std::chrono_literals;
// 		while (true) {
// 			try {
// 				roboRio->connect();
// 				logger->info("Connection to RoboRio established.");
// 				return;
// 			} catch (std::runtime_error const &e) {
// 				logger->warn("Connection to RoboRio unsuccessful. Retry.");
// 				std::this_thread::sleep_for(1s);
// 			}
// 		}
// 	}
// 	void connectJacoZed() {
// 		using namespace std::chrono_literals;
// 		while (true) {
// 			if (jacoZed->connect()) {
// 				logger->info("Connection to JacoArm established.");
// 				return;
// 			} else {
// 				logger->warn("Connection to JacoArm unsuccessful. Retry.");
// 				std::this_thread::sleep_for(1s);
// 			}
// 		}
// 	}

// 	auto processInput(Command &newInCommand) -> bool {
// 		// INPUTS
// 		Command oldInCommand = commandIn;
// 		commandIn = getInputs();

// 		if (commandIn != oldInCommand) {
// 			logger->debug("CommandHandling: event: new: {}: {}", KinovaFSM::eventNames[commandIn.event], commandIn.var);
// 		} else {
// 			logger->info("CommandHandling: event: identical");
// 		}
// 		// Check for E_Stop
// 		if (commandIn.event == KinovaFSM::E_Stop) {
// 			newInCommand = Command{KinovaFSM::E_Stop};
// 		} else {
// 			// get Hardware Error
// 			if (jacoZed->getError()) {
// 				newInCommand.event = KinovaFSM::E_Stop;
// 				logger->error("Hardware Error detected: E-Stop set.");
// 			}
// 			// get Internal HW Events
// 			else {
// 				newInCommand.event = jacoZed->getInternalEvent();
// 			}
// 			if (newInCommand.event == KinovaFSM::NoEvent) {
// 				// get Events from RoboRio
// 				newInCommand = commandIn;
// 			}
// 		}
// 		checkInputEvent(newInCommand);
// 		return kinovaSM.process(newInCommand.event, newInCommand.var);
// 	}

// 	void processOutput(bool processed, Command const &newInCommand, Command const &oldOutCommand) {
// 		KinovaFSM::Event HWEvent = jacoZed->getExternalEvent();
// 		if (HWEvent == KinovaFSM::NoEvent) {
// 			if (processed) {
// 				// Processed Event
// 				commandOut = newInCommand;
// 			}
// 			// Don't repeat last Event when HW-Event received
// 			if (newInCommand.event > KinovaFSM::numberOfNonHWEvents) {
// 				commandOut = Command{KinovaFSM::NoEvent};
// 			}
// 		} else if (HWEvent == newInCommand.event) {
// 			commandOut = Command{KinovaFSM::NoEvent};
// 		} else {
// 			commandOut = Command{HWEvent, getHWEventVar(HWEvent)};
// 		}
// 		sendOutputs(commandOut);
// 		if (commandOut != oldOutCommand) {
// 			logger->debug("CommandHandling: cmd out: {}: {}", KinovaFSM::eventNames[commandOut.event], commandOut.var);
// 		} else {
// 			logger->info("CommandHandling: cmd out: identical");
// 		}
// 	}
// };

// } // namespace KinovaZED::Control
// #endif
