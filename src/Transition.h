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
  static StateTeachSaveOrigin teachSaveOrigin;
  static StateTeachNext teachPrevious;
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
    { &initializing,    InitHomeReached,    &initializing     },
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
    { &changeMode,      NoMode,             &idle             },
    { &changeMode,      E_Stop,             &eStop            },

    { &movePosition,    SequenceDone,       &changeMode       },
    { &movePosition,    MoveJoystick,       &steering         },
    { &movePosition,    NoMode,             &idle             },
    { &movePosition,    Shutdown,           &powerOff         },
    { &movePosition,    E_Stop,             &eStop            },

    { &teach,           SetMode,            &changeModeTeach  }, 
    { &teach,           GoToPosition,       &teachMovePoint   },
    { &teach,           NoMode,             &idle             },
    { &teach,           SavePoint,          &teachSavePoint   },
    { &teach,           SaveOrigin,         &teachSaveOrigin  },
    { &teach,           Previous,           &teachPrevious    },  
    { &teach,           Next,               &teachNext        },
    { &teach,           Exit,               &changeMode       },
    { &teach,           Shutdown,           &powerOff         },
    { &teach,           E_Stop,             &eStop            },    

    { &changeModeTeach, ModeSet,            &teach            },
    { &changeModeTeach, NoMode,             &idle             },
    { &changeModeTeach, E_Stop,             &eStop            },

    { &teachMovePoint,  PointReached,       &changeModeTeach  },
    { &teachMovePoint,  NoMode,             &idle             },
    { &teachMovePoint,  E_Stop,             &eStop            },

    { &teachSavePoint,  PointSaved,         &changeModeTeach  },
    { &teachSavePoint,  PointNotSaved,      &changeModeTeach  },
    { &teachSavePoint,  NoMode,             &idle             },
    { &teachSavePoint,  E_Stop,             &eStop            },

    { &teachSaveOrigin, OriginSaved,        &changeModeTeach  },
    { &teachSaveOrigin, OriginNotSaved,     &changeModeTeach  },
    { &teachSaveOrigin, NoMode,             &idle             },
    { &teachSaveOrigin, E_Stop,             &eStop            },

    { &teachPrevious,   PreviousPointSet,   &changeModeTeach  },
    { &teachPrevious,   PreviousPointNotSet,&changeModeTeach  },
    { &teachPrevious,   NoMode,             &idle             },
    { &teachPrevious,   E_Stop,             &eStop            },

    { &teachNext,       NextPointSet,       &changeModeTeach  },
    { &teachNext,       NextPointNotSet,    &changeModeTeach  },
    { &teachNext,       NoMode,             &idle             },
    { &teachNext,       E_Stop,             &eStop            },

    { &eStop,           QuitEStop,          &powerOff         },

  };

}

#endif
