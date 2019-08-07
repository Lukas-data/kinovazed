#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Event.h"
#include "KinovaArm.h"
#include "State.h"
#include "Transition.h"

#include <memory>


/*This State Machine runs the entry, exit and tick events from the states in "State.h" according to the events and transitions in "Transition.h". There is no Queue. If an event finds no use, it is dropped.*/

constexpr auto looptime = 50.0;

struct StateMachine {
	explicit StateMachine(std::shared_ptr<KinovaArm> jacoZed);
    ~StateMachine();

    //Send Event to the StateMachine.
    void sendEvent(KinovaFSM::Event e, int eventVar = 0);

    //Run the StateMachine. Return True if StateChange is performed.
    bool process(KinovaFSM::Event e, int var);

  private:
    State* CurrentState = nullptr;
    int NumberOfTransitions = sizeof(KinovaFSM::TransitionTable)/sizeof(KinovaFSM::Transition);
    timespec LastTick;
};

#endif

