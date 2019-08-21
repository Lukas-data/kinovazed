#include "StateMachine.h"
#include "Log.h"

#include <cstdio>
#include <ctime>
#include <memory>

StateMachine::StateMachine(std::shared_ptr<KinovaArm> jacoZed) :
		currentState { KinovaFSM::initState }, numberOfTransitions { sizeof(KinovaFSM::TransitionTable) / sizeof(KinovaFSM::Transition) } {
	currentState->init(jacoZed);
	clock_gettime(CLOCK_REALTIME, &lastTick);
}

bool StateMachine::process(KinovaFSM::Event e, int var) {
	for (int i = 0; i < numberOfTransitions; i++) {
		if ((currentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event)) {
			ALL_LOG(logINFO) << "StateMachine: Processing Event '" << KinovaFSM::eventNames[e] << "'";
			KinovaFSM::TransitionTable[i].currentState->exitAction();
			currentState = KinovaFSM::TransitionTable[i].nextState;
			currentState->setEventVar(var);
			currentState->entryAction();
			return true;
		}
	}

	ALL_LOG(logDEBUG4) << "StateMachine: Not Processing Event '" << KinovaFSM::eventNames[e] << "'";
	timespec timeNow;
	clock_gettime(CLOCK_REALTIME, &timeNow);
	double elapsedTime = (timeNow.tv_sec - lastTick.tv_sec) * 1000.0 + (timeNow.tv_nsec - lastTick.tv_nsec) / 1000000.0;
	if (elapsedTime > looptime) {
		ALL_LOG(logDEBUG4) << "Tick!";
		currentState->tickAction();
		clock_gettime(CLOCK_REALTIME, &lastTick);
	}
	return false;
}

