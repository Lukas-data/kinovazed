#ifndef _STATE_H_
#define _STATE_H_

#include "KinovaArm.h"

//Abstract State-Class. (Id not used, but implemented for debug uses.)
class State {
  public:
    void init(KinovaArm* jacoZED);
	  virtual void entryAction() = 0;
    virtual void exitAction() = 0;
    virtual void tickAction() = 0;
    void setEventVar(int eventVar);

  protected:
    static KinovaArm* JacoZED;
    int EventVariable;
};


//Derived States of State-Class. 

class StatePowerOff : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateEStop : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateInitialize : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateIdle : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateChangeMode : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
/*
class StateChangeModeTranslation : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateChangeModeRotation : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateChangeModeAxis1 : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateChangeModeAxis2 : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
*/
class StateSteering : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateMovePosition : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
/*
class StateMovePositionHome : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateMovePositionBell : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
*/
class StateTeach : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateChangeModeTeach : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateTeachMovePoint : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateTeachSavePoint : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateTeachNext : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

/*
class StateTeachHome : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateTeachBell : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
*/
#endif
