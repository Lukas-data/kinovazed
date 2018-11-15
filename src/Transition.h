#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "State.h"
#include "KinovaArm.h"
#include "Event.h"

/*Handles all Transitions for "StateMachine.h".*/

namespace KinovaFSM {


//States
  static StatePowerOff powerOff;
  static StateInitialize initializing;
  static StateSteering steering;
  static StateChangeMode changeMode;
  static StateMoveToPos moveToPos;
  static StateEStop eStop;

//StartUp-State
  State* const initState = &powerOff;
  
/*
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
    SelectPosition,
    E_Stop,
    QuitEStop,
    
    //Hardware Event
    Initialized,
    ModeChanged,
    PositionReached,
    Error,

  };
*/

//Transition definition.
  struct Transition {
    State* currentState;
    Event event;
    State* nextState;
  };

//Transition Table. Next State is executed, when Current State and Transition Event match.
  static const Transition TransitionTable[] = {
    //Current         Transition        Next
    //State           Event             State
    { &powerOff,      Initialize,       &initializing   },
    { &powerOff,      E_Stop,           &eStop          },

    { &initializing,  Initialized,      &steering       },
    { &initializing,  E_Stop,           &eStop          },

    { &steering,      SelectPosition,   &moveToPos      },
    { &steering,      SetMode,          &changeMode     },
    { &steering,      Shutdown,         &powerOff       },
    { &steering,      E_Stop,           &eStop          },

    { &changeMode,    ModeChanged,      &steering       },
    { &changeMode,    Shutdown,         &powerOff       },
    { &changeMode,    E_Stop,           &eStop          },
    
    { &moveToPos,     PositionReached,  &steering       },
    { &moveToPos,     MoveJoystick,     &steering       },
    { &moveToPos,     Shutdown,         &powerOff       },
    { &moveToPos,     E_Stop,           &eStop          },

    { &eStop,         QuitEStop,        &powerOff       },

  };

}

#endif
