#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Event.h"
#include "KinovaArm.h"
#include "Logging.h"
#include "State.h"
#include "Transition.h"

#include <chrono>
#include <memory>

/*This State Machine runs the entry, exit and tick events from the states in "State.h" according to the events and
 * transitions in "Transition.h". There is no Queue. If an event finds no use, it is dropped.*/
struct StateMachine {
	explicit StateMachine(std::shared_ptr<KinovaArm> jacoZed, Logging::Logger logger);

	// Run the StateMachine. Return True if StateChange is performed.
	bool process(KinovaFSM::Event e, int var);

  private:
	Logging::Logger logger;
	State *currentState;
	int const numberOfTransitions;
	std::chrono::time_point<std::chrono::steady_clock> lastTick;
};

#endif
