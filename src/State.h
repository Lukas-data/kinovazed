#ifndef _STATE_H_
#define _STATE_H_

#include "KinovaArm.h"

//Abstract State-Class. (Id not used, but implemented for debug uses.)
class State {
  public:
    State(int id) :
      Id(id)
      {}
    int getId();
    void init(KinovaArm* jacoZED);
	  virtual void entryAction() = 0;
    virtual void exitAction() = 0;
    virtual void tickAction() = 0;

  protected:
    static KinovaArm* JacoZED;
    int Id;

};


//Derived States of State-Class. 

class StatePowerOff : public State {
  public:
    StatePowerOff() :
      State(1)
      {}
    void entryAction();
    void exitAction();
    void tickAction();

};

class StateInitialize : public State {
  public:
    StateInitialize() :
      State(2)
      {}
    void entryAction();
    void exitAction();
    void tickAction();

};

class StateSteering : public State {
  public:
    StateSteering() :
      State(3)
      {}
    void entryAction();
    void exitAction();
    void tickAction();

};

class StateChangeMode : public State {
  public:
    StateChangeMode() :
      State(4)
      {}
    void entryAction();
    void exitAction();
    void tickAction();

};

class StateMoveToPos : public State {
  public:
    StateMoveToPos() :
      State(5)
      {}
    void entryAction();
    void exitAction();
    void tickAction();
};

class StateEStop : public State {
  public:
    StateEStop() :
      State(6)
      {}
    void entryAction();
    void exitAction();
    void tickAction();
};




#endif
