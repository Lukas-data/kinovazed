#include "StateMachine.h"

#include "spdlog/spdlog.h"

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
			spdlog::info("StateMachine: Processing Event '{0}'", KinovaFSM::eventNames[e]);
			KinovaFSM::TransitionTable[i].currentState->exitAction();
			currentState = KinovaFSM::TransitionTable[i].nextState;
			currentState->setEventVar(var);
			currentState->entryAction();
			return true;
		}
	}

	spdlog::debug("StateMachine: Not Processing Event '{0}'", KinovaFSM::eventNames[e]);
	auto elapsedTime { std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTick) };
	if (elapsedTime > 50ms) {
		spdlog::debug("Tick!");
		currentState->tickAction();
		lastTick = std::chrono::steady_clock::now();
	}
	return false;
}

