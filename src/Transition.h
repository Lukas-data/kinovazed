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
  static StateChangeMode changeMode;
  static StateSteering steering;
  static StateMovePosition movePosition;
  static StateTeach teach;
  static StateChangeMode changeModeTeach;
  static StateTeachMovePoint teachMovePoint;
  static StateTeachSavePoint teachSavePoint;
  static StateTeachNext teachNext;
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
    { &powerOff,        Initialize,         &initializing     },
    { &powerOff,        E_Stop,             &eStop            },

    { &initializing,    Initialized,        &idle             },
    { &initializing,    PositionReached,    &changeMode       },
    { &initializing,    Error,              &powerOff         },
    { &initializing,    E_Stop,             &eStop            },

    { &idle,            SetMode,            &changeMode       },
    { &idle,            GoToPosition,       &movePosition     },
    { &idle,            Shutdown,           &powerOff         },
    { &idle,            E_Stop,             &eStop            },

    { &steering,        SetMode,            &changeMode       },
    { &steering,        GoToPosition,       &movePosition     },
    { &steering,        NoMode,             &idle             },
    { &steering,        Teach,              &teach            },
    { &steering,        Shutdown,           &powerOff         },
    { &steering,        E_Stop,             &eStop            },

    { &changeMode,      ModeSet,            &steering         },
    { &changeMode,      E_Stop,             &eStop            },

    { &teach,           SetMode,            &changeModeTeach  }, 
    { &teach,           GoToPosition,       &teachMovePoint   },
    { &teach,           SavePoint,          &teachSavePoint   },  
    { &teach,           Next,               &teachNext        },
    { &teach,           Exit,               &steering         },
    { &teach,           Shutdown,           &powerOff         },
    { &teach,           E_Stop,             &eStop            },    

    { &changeModeTeach, ModeSet,            &teach            },
    { &changeModeTeach, E_Stop,             &eStop            },

    { &teachMovePoint,  PositionReached,    &teach            },
    { &teachMovePoint,  E_Stop,             &eStop            },

    { &teachSavePoint,  PointSaved,         &teach            },
    { &teachSavePoint,  E_Stop,             &eStop            },

    { &teachNext,       NextPointSet,       &teach            },
    { &teachNext,       E_Stop,             &eStop            },

    { &movePosition,    PositionReached,    &changeMode       },
    { &movePosition,    MoveJoystick,       &steering         },
    { &movePosition,    NoMode,             &idle             },
    { &movePosition,    Shutdown,           &powerOff         },
    { &movePosition,    E_Stop,             &eStop            },

    { &eStop,           QuitEStop,          &powerOff         },

  };

}

#endif
