#ifndef _COMMANDHANDLING_H_
#define _COMMANDHANDLING_H_

#include "KinovaArm.h"
#include "KinovaStatus.h"
#include "Log.h"
#include "StateMachine.h"
#include "TCPServer.h"

#include <chrono>
#include <stdexcept>
#include <thread>

constexpr int numberOfJoystickMoveInputs = 3;

struct Command {
	KinovaFSM::Event event;
	int var;

	explicit Command(KinovaFSM::Event event, int var = 0) :
			event{event}, var{var} {
	}

	bool operator==(Command const &other) const {
		return event == other.event && var == other.var;
	}

	bool operator!=(Command const &other) const {
		return !(*this == other);
	}
};

struct CommandHandling {
	void init() {
		connectRoboRio();
		connectJacoZed();
		kinovaSM.init(&jacoZed);
	}
	void process() {
		Command newInCommand{KinovaFSM::NoEvent};
		bool processed = processInput(newInCommand);
		//OUTPUTS
		processOutput(processed, newInCommand, commandOut);
	}

	//Debug only:
	void debugSendEvent(KinovaFSM::Event e);
	void debugSendEvent(KinovaFSM::Event e, int var);
	void debugSetJoystick(int x, int y, int z);
	void debugPrintPosition();

private:
	KinovaArm jacoZed;
	TCPServer roboRio;
	StateMachine kinovaSM;

	Command commandOut{KinovaFSM::NoEvent};
	Command commandIn{KinovaFSM::NoEvent};

	/*Recieves Inputs from TCP Connection with RoboRIO connection. Writes Joystick Data directly to hardware.*/
	auto getInputs() -> Command {
		auto packet = roboRio.readPacket();
		Command commandIn = Command{static_cast<KinovaFSM::Event>(packet.command), packet.var};
		//TODO: (tcorbat) Shouldn't we check where whether event is actually MoveJoystick?
		jacoZed.setJoystick(packet.x, packet.y, packet.z);

		//Event is MoveJoystick, if Joystick moves and No Event is set.
		bool jSisZero = packet.x == 0 && packet.y == 0 && packet.z == 0;
		if (!jSisZero && commandIn.event != KinovaFSM::E_Stop) {
			commandIn = Command{KinovaFSM::MoveJoystick};
			ALL_LOG(logDEBUG4) << "CommandHandling::getInputs(): MoveJoystick";
		}
		return commandIn;
	}

	void sendOutputs(Command const &command) {
		RoboRioProtocol::Packet packet{command.event, command.var, 0, 0, 0};
		roboRio.sendTCP(packet);
	}

	/*define Exception for Events, that should or should not be sent to Statemachine*/
	void checkInputEvent(Command &command) {
		// 'SetMode' is only sent when mode is not already set.
		int currentMode = jacoZed.getMode();
		if (jacoZed.getActive() && command == Command{KinovaFSM::SetMode, currentMode}) {
			//printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
			command = Command{KinovaFSM::NoEvent};
		}

		//'GoToPosition' is only sent when the Arm is not already at the Position
		int currentPosition = jacoZed.getCurrentPosition();
		if (jacoZed.getActive() && command == Command{KinovaFSM::GoToPosition, currentPosition}) {
			//printf("eventVar: %d, Mode on Jaco: %d\n", eventVarToCheck, currentMode);
			command = Command{KinovaFSM::NoEvent};
		}
	}

	/*retrieves EventVariable for different HW-Events.*/
	auto getHWEventVar(KinovaFSM::Event const &event) -> int {
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

	void connectRoboRio() {
		using namespace std::chrono_literals;
		ALL_LOG(logDEBUG3) << "Trying to connect to RoboRio";
		while (true) {
			try {
				roboRio.connect();
				ALL_LOG(logINFO) << "Connection to RoboRio established.";
				return;
			} catch (std::runtime_error const &e) {
				ALL_LOG(logDEBUG1) << "Connection to RoboRio unsuccessful. Retry.";
				std::this_thread::sleep_for(1s);
			}
		}
	}
	void connectJacoZed() {
		using namespace std::chrono_literals;
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
	auto processInput(Command &newInCommand) -> bool {
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
			newInCommand = Command{KinovaFSM::E_Stop};
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

	void processOutput(bool processed, Command const &newInCommand, Command const &oldOutCommand) {
		KinovaFSM::Event HWEvent = jacoZed.getExternalEvent();
		int HWVar = getHWEventVar(HWEvent);
		if (HWEvent == KinovaFSM::NoEvent) {
			if (processed) {
				//Processed Event
				commandOut = newInCommand;
			}
			//Don't repeat last Event when HW-Event received
			if (newInCommand.event > KinovaFSM::numberOfNonHWEvents) {
				commandOut = Command{KinovaFSM::NoEvent};
			}
		} else if (HWEvent == newInCommand.event) {
			commandOut = Command{KinovaFSM::NoEvent};

		} else {
			commandOut = Command{HWEvent};
		}
		sendOutputs(commandOut);
		if (commandOut != oldOutCommand) {
			ALL_LOG(logDEBUG) << "CommandHandling: Sent Event '" << KinovaFSM::EventName[commandOut.event] << ":" << commandOut.var << "'";
		} else {
			ALL_LOG(logDEBUG4) << "CommandHandling: Sent Event '" << KinovaFSM::EventName[commandOut.event] << ":" << commandOut.var << "'";
		}
	}
};

#endif
