#ifndef _STATE_H_
#define _STATE_H_


//Abstract State-Class. (Id not used, but implemented for debug uses.)
class State {
  public:
    State();
    State(int id) :
      Id(id)
      {}
    int getId();
	  virtual void entryAction() = 0;
    virtual void exitAction() = 0;
    virtual void tickAction() = 0;

  protected:
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

class StateSteering : public State {
  public:
    StateSteering() :
      State(2)
      {}
    void entryAction();
    void exitAction();
    void tickAction();

};
/*
class StateMoveToPos : public State {
  public:
    void entryAction();
    void exitAction();
    void tickAction();
};
*/




#endif
