#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace KinovaFSM {

//Events
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
    Next,
    Exit,
    Shutdown,
    E_Stop,
    QuitEStop,
    
    //Hardware Events
    NeedsInit,
    Initialized,
    ModeSet,
    PositionReached,
    PointSaved,
    NextPointSet,
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
    "Next",
    "Exit",
    "Shutdown",
    "E_Stop",
    "QuitEStop",
    
    //Hardware Events
    "NeedsInit",
    "Initialized",
    "ModeSet",
    "PositionReached",
    "PointSaved",
    "NextPointSet",
    "Error"
  };

}

#endif
