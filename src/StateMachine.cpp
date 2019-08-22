#include "StateMachine.h"
#include "Log.h"

#include <chrono>
#include <memory>

StateMachine::StateMachine(std::shared_ptr<KinovaArm> jacoZed) :
		currentState { KinovaFSM::initState }, //
		numberOfTransitions { sizeof(KinovaFSM::TransitionTable) / sizeof(KinovaFSM::Transition) }, //
		lastTick { std::chrono::steady_clock::now() } {
	currentState->init(jacoZed);
}

bool StateMachine::process(KinovaFSM::Event e, int var) {
	using namespace std::chrono_literals;

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
	auto elapsedTime { std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTick) };
	ALL_LOG(logDEBUG4) << "StateMachine: Time since last tick " << elapsedTime.count() << "ms";
	if (elapsedTime > 50ms) {
		ALL_LOG(logDEBUG4) << "Tick!";
		currentState->tickAction();
		lastTick = std::chrono::steady_clock::now();
	}
	return false;
}

