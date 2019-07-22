#include "CommandHandling.h"
#include "Log.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <iterator>
#include <stdio.h>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

void CommandHandling::connectRoboRio() {
	ALL_LOG(logDEBUG3) << "Trying to connect to RoboRio";
	while (true) {
		if (roboRio.connect()) {
			ALL_LOG(logINFO) << "Connection to RoboRio established.";
			break;
		} else {
			ALL_LOG(logDEBUG1) << "Connection to RoboRio unsuccessful. Retry.";
		}
		std::this_thread::sleep_for(1s);
	}
}

void CommandHandling::connectJacoZed() {
	//Connect to Kinova robotic arm
	ALL_LOG(logDEBUG3) << "Trying to connect to JacoArm.";
	while (true) {
		if (jacoZed.connect()) {
			break;
		} else {
			ALL_LOG(logDEBUG1) << "Connection to JacoArm unsuccessful. Retry.";
		}
		std::this_thread::sleep_for(1s);
	}
}

void CommandHandling::init() {
	connectRoboRio();
	connectJacoZed();
	kinovaSM.init(&jacoZed);
}

auto CommandHandling::processInput(Command &newInCommand) -> bool {
	ALL_LOG(logERROR) << "process()";

	//INPUTS
	Command oldInCommand = commandIn;
	commandIn = getInputs();

	if (commandIn != oldInCommand) {
		ALL_LOG(logDEBUG) << "CommandHandling: Received Event '" << KinovaFSM::EventName[commandIn.event] << ":" << commandIn.var << "'";
	} else {
		ALL_LOG(logDEBUG4) << "CommandHandling: Received Event '" << KinovaFSM::EventName[commandIn.event] << ":" << commandIn.var << "'";
	}
	//Check for E_Stop
	if (commandIn.event == KinovaFSM::E_Stop) {
		newInCommand = Command { KinovaFSM::E_Stop };
	} else {
		//get Hardware Error
		if (jacoZed.getError()) {
			ALL_LOG(logDEBUG3) << "Hardware Error detected: E-Stop set.";
			newInCommand.event = KinovaFSM::E_Stop;
		}
		//get Internal HW Events
		else {
			ALL_LOG(logDEBUG4) << "No Hardware Error detected.";
			newInCommand.event = jacoZed.getInternalEvent();
		}
		if (newInCommand.event == KinovaFSM::NoEvent) {
			//get Events from RoboRio
			newInCommand = commandIn;
		}
	}
	checkInputEvent(newInCommand);
	kinovaSM.sendEvent(newInCommand.event, newInCommand.var);
	return kinovaSM.process();
}

void CommandHandling::processOutput(bool processed, Command const &newInCommand, Command const &oldOutCommand) {
	KinovaFSM::Event HWEvent = jacoZed.getExternalEvent();
	int HWVar = getHWEventVar(HWEvent);
	if (HWEvent == KinovaFSM::NoEvent) {
		if (processed) {
			//Processed Event
			commandOut = newInCommand;
		}
		//Don't repeat last Event when HW-Event recieved
		if (newInCommand.event > KinovaFSM::numberOfNonHWEvents) {
			commandOut = Command { KinovaFSM::NoEvent };
		}
	} else if (HWEvent == newInCommand.event) {
		commandOut = Command { KinovaFSM::NoEvent };

	} else {
		commandOut = Command { HWEvent };
	}
	sendOutputs(commandOut);
	if (commandOut != oldOutCommand) {
		ALL_LOG(logDEBUG) << "CommandHandling: Sent Event '" << KinovaFSM::EventName[commandOut.event] << ":" << commandOut.var << "'";
	} else {
		ALL_LOG(logDEBUG4) << "CommandHandling: Sent Event '" << KinovaFSM::EventName[commandOut.event] << ":" << commandOut.var << "'";
	}
}

void CommandHandling::process() {

	Command newInCommand { KinovaFSM::NoEvent };
	bool processed = processInput(newInCommand);
	//OUTPUTS
	processOutput(processed, newInCommand, commandOut);

}

/*Recieves Inputs from TCP Connection with RoboRIO connection. Writes Joystick Data directly to hardware.*/
auto CommandHandling::getInputs() -> Command {
	Command commandIn = Command { static_cast<KinovaFSM::Event>(roboRio.getCommand()), roboRio.getData(0) };
	std::array<int, numberOfJoystickMoveInputs> inData { };

	for (int n = 0; n < numberOfJoystickMoveInputs; n++) {
		inData[n] = roboRio.getData(n + 1);
	}
	//TODO: (tcorbat) Shouldn't we check where whether event is actually MoveJoystick?
	jacoZed.setJoystick(inData[0], inData[1], inData[2]);

	//Event is MoveJoystick, if Joystick moves and No Event is set.
	auto isZero = [](int value) {
		return value == 0;
	};
	bool jSisZero = std::all_of(std::begin(inData), std::end(inData), isZero);
	if (!jSisZero && commandIn.event != KinovaFSM::E_Stop) {
		commandIn = Command { KinovaFSM::MoveJoystick };
		ALL_LOG(logDEBUG4) << "CommandHandling::getInputs(): MoveJoystick";
	}
	return commandIn;
}

/*Sends Output Variables on TCP*/
void CommandHandling::sendOutputs(Command const & command) {
	roboRio.sendTCP(command.event, command.var, 0, 0, 0);
}

/*define Exception for Events, that should or should not be sent to Statemachine*/
void CommandHandling::checkInputEvent(Command &command) {
	// 'SetMode' is only sent when mode is not already set.
	int currentMode = jacoZed.getMode();
	if (jacoZed.getActive() && command == Command { KinovaFSM::SetMode, currentMode }) {
		//printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
		command = Command { KinovaFSM::NoEvent };
	}

	//'GoToPosition' is only sent when the Arm is not already at the Position
	int currentPosition = jacoZed.getCurrentPosition();
	if (jacoZed.getActive() && command == Command { KinovaFSM::GoToPosition, currentPosition }) {
		//printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
		command = Command { KinovaFSM::NoEvent };
	}
}

/*retrieves EventVariable for different HW-Events.*/
auto CommandHandling::getHWEventVar(KinovaFSM::Event const &event) -> int {
	//Hardware Events
	switch (event) {
	case KinovaFSM::ModeSet:
		return jacoZed.getMode();
	case KinovaFSM::SequenceDone:
		return jacoZed.getCurrentPosition();
	case KinovaFSM::PointReached:
	case KinovaFSM::PointSaved:
	case KinovaFSM::PointNotSaved:
	case KinovaFSM::PointDeleted:
	case KinovaFSM::PointNotDeleted:
	case KinovaFSM::PreviousPointNotSet:
	case KinovaFSM::PreviousPointSet:
	case KinovaFSM::NextPointNotSet:
	case KinovaFSM::NextPointSet:
		return jacoZed.getCurrentPoint();
	}
	return 0;
}

/************************************/
//* Debug use only! */
void CommandHandling::debugSendEvent(KinovaFSM::Event e) {
	kinovaSM.sendEvent(e);
}

void CommandHandling::debugSendEvent(KinovaFSM::Event e, int var) {
	kinovaSM.sendEvent(e, var);
}

void CommandHandling::debugSetJoystick(int x, int y, int z) {
	//printf("CommandHandling::setJoystick (%d,%d,%d)\n",x,y,z);
	jacoZed.setJoystick(x, y, z);
}

void CommandHandling::debugPrintPosition() {
	float coordinates[6];
	jacoZed.getPosition(coordinates);
	printf("Current Position = (");
	for (int i = 0; i < 6; i++) {
		printf("%f", coordinates[i]);
		if (i < 5) {
			printf(", ");
		}
	}
	printf(").\n");
}

