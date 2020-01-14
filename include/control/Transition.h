#ifndef INCLUDE_CONTROL_TRANSITION_H_
#define INCLUDE_CONTROL_TRANSITION_H_

#include "control/Event.h"
#include "control/State.h"
#include "hw/KinovaArm.h"

namespace KinovaZED::Control {

// States
static StatePowerOff powerOff;
static StateInitialize initializing;
static StateRetract retract;
static StateUnfold unfolding;
static StateIdle idle;
static StateChangeMode changeMode;
static StateChangeModeDefault changeModeDef;
static StateSteering steering;
static StateMovePosition movePosition;
static StateTeach teach;
static StateChangeMode changeModeTeach;
static StateTeachMovePoint teachMovePoint;
static StateTeachMoveOrigin teachMoveOrigin;
static StateTeachSavePoint teachSavePoint;
static StateTeachSaveOrigin teachSaveOrigin;
static StateTeachDeletePoint teachDeletePnt;
static StateTeachPrevious teachPrevious;
static StateTeachNext teachNext;
static StateEStop eStop;

// StartUp-State
State *const initState = &powerOff;

// Transition definition.
struct Transition {
	State *currentState;
	Event event;
	State *nextState;
};

// Transition Table. Next State is executed, when Current State and Transition Event match.
static const Transition TransitionTable[] = {
    // Current           Transition          Next
    // State             Event               State
    {&powerOff, Event::Initialize, &initializing},
    {&powerOff, Event::E_Stop, &eStop},

    {&initializing, Event::Initialized, &retract},
    {&initializing, Event::InitHomeReached, &initializing},
    {&initializing, Event::Error, &powerOff},
    {&initializing, Event::E_Stop, &eStop},

    {&retract, Event::Unfold, &unfolding},
    {&retract, Event::E_Stop, &eStop},
    {&retract, Event::Shutdown, &powerOff},

    {&unfolding, Event::Unfolded, &idle},
    {&unfolding, Event::E_Stop, &eStop},
    {&unfolding, Event::Shutdown, &powerOff},

    {&idle, Event::SetMode, &changeMode},
    {&idle, Event::GoToPosition, &movePosition},
    {&idle, Event::Retract, &retract},
    {&idle, Event::Shutdown, &powerOff},
    {&idle, Event::E_Stop, &eStop},

    {&steering, Event::SetMode, &changeMode},
    {&steering, Event::GoToPosition, &movePosition},
    {&steering, Event::NoMode, &idle},
    {&steering, Event::Retract, &retract},
    {&steering, Event::Teach, &teach},
    {&steering, Event::Shutdown, &powerOff},
    {&steering, Event::E_Stop, &eStop},

    {&changeMode, Event::ModeSet, &steering},
    {&changeMode, Event::NoMode, &idle},
    {&changeMode, Event::Retract, &retract},
    {&changeMode, Event::E_Stop, &eStop},

    {&changeModeDef, Event::ModeSet, &steering},
    {&changeModeDef, Event::NoMode, &idle},
    {&changeModeDef, Event::Retract, &retract},
    {&changeModeDef, Event::E_Stop, &eStop},

    {&movePosition, Event::SequenceDone, &changeMode},
    {&movePosition, Event::MoveJoystick, &changeModeDef},
    {&movePosition, Event::NoMode, &idle},
    {&movePosition, Event::Retract, &retract},
    {&movePosition, Event::Shutdown, &powerOff},
    {&movePosition, Event::E_Stop, &eStop},

    {&teach, Event::SetMode, &changeModeTeach},
    {&teach, Event::GoToPosition, &teachMovePoint},
    {&teach, Event::MoveOrigin, &teachMoveOrigin},
    {&teach, Event::NoMode, &idle},
    {&teach, Event::Retract, &retract},
    {&teach, Event::SavePoint, &teachSavePoint},
    {&teach, Event::SaveOrigin, &teachSaveOrigin},
    {&teach, Event::DeletePoint, &teachDeletePnt},
    {&teach, Event::Previous, &teachPrevious},
    {&teach, Event::Next, &teachNext},
    {&teach, Event::Exit, &changeMode},
    {&teach, Event::Shutdown, &powerOff},
    {&teach, Event::E_Stop, &eStop},

    {&changeModeTeach, Event::ModeSet, &teach},
    {&changeModeTeach, Event::NoMode, &idle},
    {&changeModeTeach, Event::Retract, &retract},
    {&changeModeTeach, Event::E_Stop, &eStop},

    {&teachMovePoint, Event::PointReached, &changeModeTeach},
    {&teachMovePoint, Event::PointNotReached, &changeModeTeach},
    {&teachMovePoint, Event::MoveJoystick, &changeModeTeach},
    {&teachMovePoint, Event::NoMode, &idle},
    {&teachMovePoint, Event::Retract, &retract},
    {&teachMovePoint, Event::E_Stop, &eStop},

    {&teachMoveOrigin, Event::PointReached, &changeModeTeach},
    {&teachMoveOrigin, Event::PointNotReached, &changeModeTeach},
    {&teachMoveOrigin, Event::MoveJoystick, &changeModeTeach},
    {&teachMoveOrigin, Event::NoMode, &idle},
    {&teachMoveOrigin, Event::Retract, &retract},
    {&teachMoveOrigin, Event::E_Stop, &eStop},

    {&teachSavePoint, Event::PointSaved, &changeModeTeach},
    {&teachSavePoint, Event::PointNotSaved, &changeModeTeach},
    {&teachSavePoint, Event::NoMode, &idle},
    {&teachSavePoint, Event::Retract, &retract},
    {&teachSavePoint, Event::E_Stop, &eStop},

    {&teachSaveOrigin, Event::OriginSaved, &changeModeTeach},
    {&teachSaveOrigin, Event::OriginNotSaved, &changeModeTeach},
    {&teachSaveOrigin, Event::NoMode, &idle},
    {&teachSaveOrigin, Event::Retract, &retract},
    {&teachSaveOrigin, Event::E_Stop, &eStop},

    {&teachDeletePnt, Event::PointDeleted, &changeModeTeach},
    {&teachDeletePnt, Event::PointNotDeleted, &changeModeTeach},
    {&teachDeletePnt, Event::NoMode, &idle},
    {&teachDeletePnt, Event::Retract, &retract},
    {&teachDeletePnt, Event::E_Stop, &eStop},

    {&teachPrevious, Event::PreviousPointSet, &changeModeTeach},
    {&teachPrevious, Event::PreviousPointNotSet, &changeModeTeach},
    {&teachPrevious, Event::NoMode, &idle},
    {&teachPrevious, Event::Retract, &retract},
    {&teachPrevious, Event::E_Stop, &eStop},

    {&teachNext, Event::NextPointSet, &changeModeTeach},
    {&teachNext, Event::NextPointNotSet, &changeModeTeach},
    {&teachNext, Event::NoMode, &idle},
    {&teachNext, Event::Retract, &retract},
    {&teachNext, Event::E_Stop, &eStop},

    {&eStop, Event::QuitEStop, &powerOff},
};

} // namespace KinovaZED::Control

#endif
