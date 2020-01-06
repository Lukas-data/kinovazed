#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "Event.h"
#include "KinovaArm.h"
#include "State.h"

/*Handles all Transitions for "StateMachine.h".*/

namespace KinovaFSM {


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
    {&powerOff, Initialize, &initializing},
    {&powerOff, E_Stop, &eStop},

    {&initializing, Initialized, &retract},
    {&initializing, InitHomeReached, &initializing},
    {&initializing, Error, &powerOff},
    {&initializing, E_Stop, &eStop},

    {&retract, Unfold, &unfolding},
    {&retract, E_Stop, &eStop},
    {&retract, Shutdown, &powerOff},

    {&unfolding, Unfolded, &idle},
    {&unfolding, E_Stop, &eStop},
    {&unfolding, Shutdown, &powerOff},

    {&idle, SetMode, &changeMode},
    {&idle, GoToPosition, &movePosition},
    {&idle, Retract, &retract},
    {&idle, Shutdown, &powerOff},
    {&idle, E_Stop, &eStop},

    {&steering, SetMode, &changeMode},
    {&steering, GoToPosition, &movePosition},
    {&steering, NoMode, &idle},
    {&steering, Retract, &retract},
    {&steering, Teach, &teach},
    {&steering, Shutdown, &powerOff},
    {&steering, E_Stop, &eStop},

    {&changeMode, ModeSet, &steering},
    {&changeMode, NoMode, &idle},
    {&changeMode, Retract, &retract},
    {&changeMode, E_Stop, &eStop},

    {&changeModeDef, ModeSet, &steering},
    {&changeModeDef, NoMode, &idle},
    {&changeModeDef, Retract, &retract},
    {&changeModeDef, E_Stop, &eStop},

    {&movePosition, SequenceDone, &changeMode},
    {&movePosition, MoveJoystick, &changeModeDef},
    {&movePosition, NoMode, &idle},
    {&movePosition, Retract, &retract},
    {&movePosition, Shutdown, &powerOff},
    {&movePosition, E_Stop, &eStop},

    {&teach, SetMode, &changeModeTeach},
    {&teach, GoToPosition, &teachMovePoint},
    {&teach, MoveOrigin, &teachMoveOrigin},
    {&teach, NoMode, &idle},
    {&teach, Retract, &retract},
    {&teach, SavePoint, &teachSavePoint},
    {&teach, SaveOrigin, &teachSaveOrigin},
    {&teach, DeletePoint, &teachDeletePnt},
    {&teach, Previous, &teachPrevious},
    {&teach, Next, &teachNext},
    {&teach, Exit, &changeMode},
    {&teach, Shutdown, &powerOff},
    {&teach, E_Stop, &eStop},

    {&changeModeTeach, ModeSet, &teach},
    {&changeModeTeach, NoMode, &idle},
    {&changeModeTeach, Retract, &retract},
    {&changeModeTeach, E_Stop, &eStop},

    {&teachMovePoint, PointReached, &changeModeTeach},
    {&teachMovePoint, PointNotReached, &changeModeTeach},
    {&teachMovePoint, MoveJoystick, &changeModeTeach},
    {&teachMovePoint, NoMode, &idle},
    {&teachMovePoint, Retract, &retract},
    {&teachMovePoint, E_Stop, &eStop},

    {&teachMoveOrigin, PointReached, &changeModeTeach},
    {&teachMoveOrigin, PointNotReached, &changeModeTeach},
    {&teachMoveOrigin, MoveJoystick, &changeModeTeach},
    {&teachMoveOrigin, NoMode, &idle},
    {&teachMoveOrigin, Retract, &retract},
    {&teachMoveOrigin, E_Stop, &eStop},

    {&teachSavePoint, PointSaved, &changeModeTeach},
    {&teachSavePoint, PointNotSaved, &changeModeTeach},
    {&teachSavePoint, NoMode, &idle},
    {&teachSavePoint, Retract, &retract},
    {&teachSavePoint, E_Stop, &eStop},

    {&teachSaveOrigin, OriginSaved, &changeModeTeach},
    {&teachSaveOrigin, OriginNotSaved, &changeModeTeach},
    {&teachSaveOrigin, NoMode, &idle},
    {&teachSaveOrigin, Retract, &retract},
    {&teachSaveOrigin, E_Stop, &eStop},

    {&teachDeletePnt, PointDeleted, &changeModeTeach},
    {&teachDeletePnt, PointNotDeleted, &changeModeTeach},
    {&teachDeletePnt, NoMode, &idle},
    {&teachDeletePnt, Retract, &retract},
    {&teachDeletePnt, E_Stop, &eStop},

    {&teachPrevious, PreviousPointSet, &changeModeTeach},
    {&teachPrevious, PreviousPointNotSet, &changeModeTeach},
    {&teachPrevious, NoMode, &idle},
    {&teachPrevious, Retract, &retract},
    {&teachPrevious, E_Stop, &eStop},

    {&teachNext, NextPointSet, &changeModeTeach},
    {&teachNext, NextPointNotSet, &changeModeTeach},
    {&teachNext, NoMode, &idle},
    {&teachNext, Retract, &retract},
    {&teachNext, E_Stop, &eStop},

    {&eStop, QuitEStop, &powerOff},
};

} // namespace KinovaFSM

#endif
