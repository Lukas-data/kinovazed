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
  static StateIdle idle;
  static StateChangeModeTranslation toTranslation;
  static StateChangeModeRotation toRotation;
  static StateChangeModeAxis1 toAxis1;
  static StateChangeModeAxis2 toAxis2;
  static StateSteering steering;
  static StateMoveTrajectory moveTrajectory;
  static StateEStop eStop;

//StartUp-State
  State* const initState = &powerOff;


//Transition definition.
  struct Transition {
    State* currentState;
    Event event;
    State* nextState;
  };

//Transition Table. Next State is executed, when Current State and Transition Event match.
  static const Transition TransitionTable[] = {
    //Current           Transition          Next
    //State             Event               State
    { &powerOff,        Initialize,         &initializing   },
    { &powerOff,        E_Stop,             &eStop          },

    { &initializing,    Initialized,        &idle           },
    { &initializing,    E_Stop,             &eStop          },

    { &idle,            SetModeTranslation, &toTranslation  },
    { &idle,            SetModeRotation,    &toRotation     },
    { &idle,            SetModeAxis1,       &toAxis1        },
    { &idle,            SetModeAxis2,       &toAxis2        },
    { &idle,            SelectPosition,     &moveTrajectory },
    { &idle,            Shutdown,           &powerOff       },
    { &idle,            E_Stop,             &eStop          },

    { &steering,        SelectPosition,     &moveTrajectory },
    { &steering,        SetModeTranslation, &toTranslation  },
    { &steering,        SetModeRotation,    &toRotation     },
    { &steering,        SetModeAxis1,       &toAxis1        },
    { &steering,        SetModeAxis2,       &toAxis2        },
    { &steering,        NoMode,             &idle           },
    { &steering,        Shutdown,           &powerOff       },
    { &steering,        E_Stop,             &eStop          },

    {&toTranslation,    ModeTranslation,    &steering       },
    {&toRotation,       ModeRotation,       &steering       },
    {&toAxis1,          ModeAxis1,          &steering       },
    {&toAxis2,          ModeAxis2,          &steering       },

    { &moveTrajectory,  PositionReached,    &steering       },
    { &moveTrajectory,  MoveJoystick,       &steering       },
    { &moveTrajectory,  NoMode,             &idle           },
    { &moveTrajectory,  Shutdown,           &powerOff       },
    { &moveTrajectory,  E_Stop,             &eStop          },

    { &eStop,           QuitEStop,          &powerOff       },

  };

}

#endif
