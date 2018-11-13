#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "State.h"

/*Handles all Events and Transitions for "StateMachine.h".*/

namespace KinovaFSM {

//States
  static StatePowerOff powerOff;
  static StateSteering steering;

//StartUp-State
  State* const initState = &powerOff;

//Events
  enum Event {
    //System Event
    NoEvent,
    Tick,
    
    //User Event
    Initialize,
    SetMode,
    MoveJoystick,
    Shutdown,
    
    //Hardware Event
    Initialized,
    PositionReached,

  };

//Transition definition.
  struct Transition {
    State* currentState;
    Event event;
    State* nextState;
  };

//Transition Table. Next State is executed, when Current State and Transition Event match.
  static const Transition TransitionTable[] = {
    //Current     Transition    Next
    //State       Event         State
    { &powerOff,  Initialize,   &steering },
    { &steering,  Shutdown,     &powerOff },
  };

}

#endif
