#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace KinovaFSM {

  static const int numberOfNonHWEvents = 19; //Not including NoEvent.
//Events !Hardware Events at the End!
  enum Event {
    //System Events
    NoEvent,
    Tick,
    
    //User Events
    Initialize,
    NoMode,
    Retract,
    Unfold,
    SetMode,
    MoveJoystick,
    GoToPosition,
    Teach,
    SavePoint,
    SaveOrigin,
    DeletePoint,
    Previous,
    Next,
    MoveOrigin,
    Exit,
    Shutdown,
    E_Stop,
    QuitEStop,
    
    //Hardware Events Communicated To RoboRio (external)
    Initialized,
    Unfolded,
    ModeSet,
    SequenceDone,
    PointReached,
    PointNotReached,
    PointSaved,
    PointNotSaved,
    OriginSaved,
    OriginNotSaved,
    PointDeleted,
    PointNotDeleted,
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
    "Retract",
    "Unfold",
    "SetMode",
    "MoveJoystick",
    "GoToPosition",
    "Teach",
    "SavePoint",
    "SaveOrigin",
    "DeletePoint",
    "Previous",
    "Next",
    "MoveOrigin",
    "Exit",
    "Shutdown",
    "E_Stop",
    "QuitEStop",
    
    //Hardware Events Communicated
    "Initialized",
    "Unfolded",
    "ModeSet",
    "SequenceDone",
    "PointReached",
    "PointNotReached",
    "PointSaved",
    "PointNotSaved",
    "OriginSaved",
    "OriginNotSaved",
    "PointDeleted",
    "PointNotDeleted",
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
