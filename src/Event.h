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
    //SetModeTranslation,
    //SetModeRotation,
    //SetModeAxis1,
    //SetModeAxis2,

    MoveJoystick,

    GoToPosition,
    //GoToPositionHome,
    //GoToPositionBell,

    Teach,
    SavePoint,
    Next,
    //TeachMove,
    //TeachSave,
    Exit,
    Shutdown,
    E_Stop,
    QuitEStop,
    
    //Hardware Events
    Initialized,

    //ModeTranslation,
    //ModeRotation,
    //ModeAxis1,
    //ModeAxis2,
    ModeSet,
  
    PositionReached,
    
    PointSaved,
    NextPointSet,
    //TeachPointReached,
    //TeachPointSaved,

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
    //"SetModeTranslation",
    //"SetModeRotation",
    //"SetModeAxis1",
    //"SetModeAxis2",

    "MoveJoystick",

    "GoToPosition",
    //"GoToPositionHome",
    //"GoToPositionBell",

    "Teach",
    "SavePoint",
    "Next",
    //"TeachMove",
    //"TeachSave",
    "Exit",
    "Shutdown",
    "E_Stop",
    "QuitEStop",
    
    //Hardware Events
    "Initialized",
    //"ModeTranslation",
    //"ModeRotation",
    //"ModeAxis1",
    //"ModeAxis2",
    "ModeSet",

    "PositionReached",

    "PointSaved",
    "NextPointSet",

    //"TeachPointReached",
    //"TeachPointSaved",

    "Error"
  };

}

#endif
