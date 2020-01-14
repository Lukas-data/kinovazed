#ifndef INCLUDE_CONTROL_COMMAND_HANDLING_H_
#define INCLUDE_CONTROL_COMMAND_HANDLING_H_

#include "comm/CommandInterface.h"
#include "comm/CommandSubscriber.h"
#include "control/StateMachine.h"
#include "hw/Actor.h"
#include "support/Logging.h"

constexpr int numberOfJoystickMoveInputs = 3;

namespace KinovaZED::Control {

struct CommandHandling : Comm::CommandSubscriber {
	CommandHandling(Comm::CommandInterface &interface, Hw::Actor &actor, Logger logger);

	auto process() -> void;

  private:
	Comm::CommandInterface &commandSource;
	Hw::Actor &arm;
	Logger logger;
	StateMachine kinovaSM;

	// Comm::Command commandOut{};
	// Comm::Command commandIn{};

	/*Recieves Inputs from TCP Connection with RoboRIO connection. Writes Joystick Data directly to hardware.*/
	auto getInputs() -> void;
	auto sendOutputs(Comm::Command const &command) -> void;
	auto checkInputEvent(Comm::Command &command) -> void;
	auto getHWEventVar(Event const &event) -> int;

	auto connectRoboRio() -> void;
	auto connectJacoZed() -> void;

	auto processInput(Comm::Command &newInCommand) -> bool;

	auto processOutput(bool processed, Comm::Command const &newInCommand, Comm::Command const &oldOutCommand) -> void;
};

} // namespace KinovaZED::Control
#endif
