#ifndef _COMMANDHANDLING_H_
#define _COMMANDHANDLING_H_

#include "KinovaArm.h"
#include "KinovaStatus.h"
#include "StateMachine.h"
#include "TCPServer.h"

constexpr int numberOfJoystickMoveInputs = 3;

struct Command {
	KinovaFSM::Event event;
	int var;

	explicit Command(KinovaFSM::Event event, int var = 0) : event{event}, var{var}{}

	bool operator==(Command const & other) const {
		return event == other.event && var == other.var;
	}

	bool operator!=(Command const & other) const {
		return !(*this == other);
	}
};

struct CommandHandling {
	void init();
	void process();

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

	auto getInputs() -> Command;
	void sendOutputs(int event, int eventVar);
	void checkInputEvent(Command & command);
	auto getHWEventVar(KinovaFSM::Event const &event) -> int;
	void connectRoboRio();
	void connectJacoZed();
	bool processInput(Command & newInCommand);
	void processOutput(bool processed, Command const & newInCommand, Command const & oldOutCommand);
};
#endif
