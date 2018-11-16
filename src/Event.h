#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace KinovaFSM {

//Events
  enum Event {
    //System Event
    NoEvent,
    Tick,
    
    //User Event
    Initialize,
    NoMode,
    SetModeTranslation,
    SetModeRotation,
    SetModeAxis1,
    SetModeAxis2,
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

  //ForDebugUse
  static const char *EventName[] =
  {
    //System Event
    "NoEvent",
    "Tick",
    
    //User Event
    "Initialize",
    "NoMode",
    "SetModeTranslation",
    "SetModeRotation",
    "SetModeAxis1",
    "SetModeAxis2",
    "MoveJoystick",
    "Shutdown",
    "SelectPosition",
    "E_Stop",
    "QuitEStop",
    
    //Hardware Event
    "Initialized",
    "ModeChanged",
    "PositionReached",
    "Error"
  };

}

#endif
