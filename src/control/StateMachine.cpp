#include "control/StateMachine.h"

#include "support/Logging.h"

#include <chrono>
#include <memory>

namespace KinovaZED::Control {

StateMachine::StateMachine(std::shared_ptr<Hw::KinovaArm> jacoZed, Logger logger)
    : logger{logger}
    , currentState{initState}
    , numberOfTransitions{sizeof(TransitionTable) / sizeof(Transition)}
    , lastTick{std::chrono::steady_clock::now()} {
	currentState->init(jacoZed);
}

bool StateMachine::process(Event e, int var) {
	using namespace std::chrono_literals;

	for (int i = 0; i < numberOfTransitions; i++) {
		if ((currentState == TransitionTable[i].currentState) && (e == TransitionTable[i].event)) {
			logger->info("StateMachine: Processing Event '{0}'", eventNames[static_cast<int>(e)]);
			logger->info("Exiting State {0}", TransitionTable[i].currentState->getName());
			TransitionTable[i].currentState->exitAction();
			currentState = TransitionTable[i].nextState;
			currentState->setEventVar(var);
			logger->info("Entering State {0}", currentState->getName());
			currentState->entryAction();
			return true;
		}
	}

	logger->debug("StateMachine: Not Processing Event '{0}'", eventNames[static_cast<int>(e)]);
	auto elapsedTime{
	    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lastTick)};
	if (elapsedTime > 50ms) {
		logger->debug("Tick!");
		currentState->tickAction();
		lastTick = std::chrono::steady_clock::now();
	}
	return false;
}

} // namespace KinovaZED::Control