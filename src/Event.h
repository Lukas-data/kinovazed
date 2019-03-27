#ifndef _EVENT_H_
#define _EVENT_H_

/*Handles all Events for "StateMachine.h".*/

namespace KinovaFSM {

  static const int numberOfNonHWEvents = 19; //Not including NoEvent.
//Events !Hardware Events at the End!
  enum Event {
    //System Events
    NoEvent, //0
    Tick,    //1 
    
    //User Events
    Initialize,   //2
    NoMode,       //3
    Retract,      //4
    Unfold,       //5
    SetMode,      //6
    MoveJoystick, //7
    GoToPosition, //8
    Teach,        //9
    SavePoint,    //10
    SaveOrigin,   //11
    DeletePoint,  //12
    Previous,     //13
    Next,         //14
    MoveOrigin,   //15
    Exit,         //16
    Shutdown,     //17  
    E_Stop,       //18
    QuitEStop,    //19
    
    //Hardware Events Communicated To RoboRio (external)
    Initialized,          //20
    Retracted,            //21
    Unfolded,             //22
    ModeSet,              //23
    SequenceDone,         //24
    PointReached,         //25
    PointNotReached,      //26
    PointSaved,           //27
    PointNotSaved,        //28
    OriginSaved,          //29
    OriginNotSaved,       //30
    PointDeleted,         //31
    PointNotDeleted,      //32
    PreviousPointSet,     //33
    PreviousPointNotSet,  //34
    NextPointSet,         //35
    NextPointNotSet,      //36

    //Hardware Events Uncommunicated (internal)
    InitHomeReached,      //37
    Error,                //38
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
    "Retracted",
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
