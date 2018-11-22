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
  //static StateChangeModeTranslation toTranslation;
  //static StateChangeModeRotation toRotation;
  //static StateChangeModeAxis1 toAxis1;
  //static StateChangeModeAxis2 toAxis2;
  static StateSteering steering;
  static StateMovePosition movePosition;
  //static StateMovePositionHome toHome;
  //static StateMovePositionBell toBell;
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
    { &initializing,    Error,              &powerOff         },
    { &initializing,    E_Stop,             &eStop            },

    { &idle,            SetMode,            &changeMode       },
//    { &idle,            SetModeTranslation, &toTranslation  },
//    { &idle,            SetModeRotation,    &toRotation     },
//    { &idle,            SetModeAxis1,       &toAxis1        },
//    { &idle,            SetModeAxis2,       &toAxis2        },
    { &idle,            GoToPosition,       &movePosition     },
//    { &idle,            GoToPositionHome,   &toHome         },
//    { &idle,            GoToPositionBell,   &toBell         },
    { &idle,            Shutdown,           &powerOff         },
    { &idle,            E_Stop,             &eStop            },

    { &steering,        SetMode,            &changeMode       },
//    { &steering,        SetModeTranslation, &toTranslation  },
//    { &steering,        SetModeRotation,    &toRotation     },
//    { &steering,        SetModeAxis1,       &toAxis1        },
//    { &steering,        SetModeAxis2,       &toAxis2        },
    { &steering,        GoToPosition,       &movePosition     },
//    { &steering,        GoToPositionHome,   &toHome         },
//    { &steering,        GoToPositionBell,   &toBell         },
    { &steering,        NoMode,             &idle             },
    { &steering,        Teach,              &teach            },
    { &steering,        Shutdown,           &powerOff         },
    { &steering,        E_Stop,             &eStop            },

    { &changeMode,      ModeSet,            &steering         },
//    {&toTranslation,    ModeTranslation,    &steering       },
//    {&toRotation,       ModeRotation,       &steering       },
//    {&toAxis1,          ModeAxis1,          &steering       },
//    {&toAxis2,          ModeAxis2,          &steering       },

    { &teach,           SetMode,            &changeModeTeach  }, 
    { &teach,           GoToPosition,       &teachMovePoint   },
    { &teach,           SavePoint,          &teachSavePoint   },  
    { &teach,           Next,               &teachNext        },
    { &teach,           Exit,               &steering         },

    { &changeModeTeach, ModeSet,            &teach            },
    { &teachMovePoint,  PositionReached,    &teach            },
    { &teachSavePoint,  PointSaved,         &teach            },
    { &teachNext,       NextPointSet,       &teach            },

    { &movePosition,    PositionReached,    &changeMode       },
    { &movePosition,    MoveJoystick,       &steering         },
    { &movePosition,    NoMode,             &idle             },
    { &movePosition,    Shutdown,           &powerOff         },
    { &movePosition,    E_Stop,             &eStop            },

/*
//    {&toHome,           PositionReached,    &toTranslation  },
    {&toHome,           PositionReached,    &changeMode     },
    {&toHome,           MoveJoystick,       &steering       },
    {&toHome,           NoMode,             &idle           },
    {&toHome,           Shutdown,           &powerOff       },
    {&toHome,           E_Stop,             &eStop          },

//    {&toBell,           PositionReached,    &toTranslation  },
    {&toBell,           PositionReached,    &changeMode     },
    {&toBell,           MoveJoystick,       &steering       },
    {&toBell,           NoMode,             &idle           },
    {&toBell,           Shutdown,           &powerOff       },
    {&toBell,           E_Stop,             &eStop          },
*/

    { &eStop,           QuitEStop,          &powerOff       },

  };

}

#endif
