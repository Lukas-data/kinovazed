#ifndef _STATE_H_
#define _STATE_H_

#include "KinovaArm.h"

#include <memory>
#include <string>
#include <utility>

struct State {
	void init(std::shared_ptr<KinovaArm> jacoZed);
	void entryAction();
	void exitAction();
	virtual void tickAction();

	void setEventVar(int eventVar);
	std::string getName() const;

	virtual ~State() noexcept = default;

  protected:
	explicit State(std::string name)
	    : name{std::move(name)} {
	}

	static std::shared_ptr<KinovaArm> JacoZED;
	int eventVariable{};
	std::string name;

	virtual void entryActionHook() {
	}
	virtual void exitActionHook() {
	}
};

// Derived States of State-Class.

struct StatePowerOff : State {
	StatePowerOff()
	    : State{"PowerOff"} {
	}
	void entryActionHook();
};

struct StateEStop : State {
	StateEStop()
	    : State{"EStop"} {
	}
	void entryActionHook();
	void tickAction();
};

struct StateInitialize : State {
	StateInitialize()
	    : State{"Initialize"} {
	}
	void entryActionHook();
	void exitActionHook();
	void tickAction();
};

struct StateRetract : State {
	StateRetract()
	    : State{"Retract"} {
	}
	void entryActionHook();
	void tickAction();
};

struct StateUnfold : State {
	StateUnfold()
	    : State{"Unfolding"} {
	}
	void exitActionHook();
	void tickAction();
};

struct StateIdle : State {
	StateIdle()
	    : State{"Idle"} {
	}
	void entryActionHook();
	void exitActionHook();
};

struct StateChangeMode : State {
	StateChangeMode()
	    : State{"ChangeMode"} {
	}
	void entryActionHook();
	void tickAction();
};

struct StateChangeModeDefault : State {
	StateChangeModeDefault()
	    : State{"ChangeModeDefault"} {
	}
	void entryActionHook();
	void tickAction();
};

struct StateSteering : State {
	StateSteering()
	    : State{"Steering"} {
	}
	void exitActionHook();
	void tickAction();
};

struct StateMovePosition : State {
	StateMovePosition()
	    : State{"MovePosition"} {
	}
	void entryActionHook();
	void exitActionHook();
	void tickAction();
};

struct StateTeach : State {
	StateTeach()
	    : State{"Teach"} {
	}
	void entryActionHook();
	void exitActionHook();
	void tickAction();
};

struct StateChangeModeTeach : State {
	StateChangeModeTeach()
	    : State{"ChangeModeTeach"} {
	}
	void entryActionHook();
	void exitAction();
	void tickAction();
};

struct StateTeachMovePoint : State {
	StateTeachMovePoint()
	    : State{"TeachMovePoint"} {
	}
	void exitActionHook();
	void tickAction();
};

struct StateTeachMoveOrigin : State {
	StateTeachMoveOrigin()
	    : State{"TeachMoveOrigin"} {
	}
	void exitActionHook();
	void tickAction();
};

struct StateTeachSavePoint : State {
	StateTeachSavePoint()
	    : State{"TeachSavePoint"} {
	}
	void entryActionHook();
};

struct StateTeachSaveOrigin : State {
	StateTeachSaveOrigin()
	    : State{"TeachSaveOrigin"} {
	}
	void entryActionHook();
};

struct StateTeachDeletePoint : State {
	StateTeachDeletePoint()
	    : State{"TeachDeletePoint"} {
	}
	void entryActionHook();
};

struct StateTeachPrevious : State {
	StateTeachPrevious()
	    : State{"TeachPrevious"} {
	}
	void entryActionHook();
};

struct StateTeachNext : State {
	StateTeachNext()
	    : State{"TeachNext"} {
	}
	void entryActionHook();
};

#endif
