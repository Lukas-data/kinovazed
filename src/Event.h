#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace KinovaFSM {

  static const int numberOfNonHWEvents = 16; //Not including NoEvent.
//Events !Hardware Events at the End!
  enum Event {
    //System Events
    NoEvent,
    Tick,
    
    //User Events
    Initialize,
    NoMode,
    SetMode,
    MoveJoystick,
    GoToPosition,
    Teach,
    SavePoint,
    SaveOrigin,
    Previous,
    Next,
    MoveOrigin,
    Exit,
    Shutdown,
    E_Stop,
    QuitEStop,
    
    //Hardware Events Communicated To RoboRio (external)
    Initialized,
    ModeSet,
    SequenceDone,
    PointReached,
    PointNotReached,
    PointSaved,
    PointNotSaved,
    OriginSaved,
    OriginNotSaved,
    PreviousPointSet,
    PreviousPointNotSet,
    NextPointSet,
    NextPointNotSet,

    //Hardware Events Uncommunicated (internal)
    InitHomeReached,
    Error,
  };

  //ForDebugUse
  static const char *EventName[] =
  {
    //System Events
    "NoEvent",
    "Tick",
    
    //User Events
    "Initialize",
    "NoMode",
    "SetMode",
    "MoveJoystick",
    "GoToPosition",
    "Teach",
    "SavePoint",
    "SaveOrigin",
    "Previous",
    "Next",
    "MoveOrigin",
    "Exit",
    "Shutdown",
    "E_Stop",
    "QuitEStop",
    
    //Hardware Events Communicated
    "Initialized",
    "ModeSet",
    "SequenceDone",
    "PointReached",
    "PointNotReached",
    "PointSaved",
    "PointNotSaved",
    "OriginSaved",
    "OriginNotSaved",
    "PreviousPointSet",
    "PreviousPointNotSet",
    "NextPointSet",
    "NextPointNotSet",

    //Hardware Events Uncommunicated
    "InitHomeReached",
    "Error"
  };

}

#endif
