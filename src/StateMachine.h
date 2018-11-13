#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "State.h"
#include "Transition.h"

/*This State Machine runs the entry, exit and tick events from the states in "State.h" according to the events and transitions in "Transition.h". There is no Queue. If an event finds no use, it is dropped.*/

class StateMachine {

  public:
    StateMachine() :
      //Calculates the Number Of Transitions in the Transition Table as it is assumed unchanged during runtime.
      numberOfTransitions(sizeof(KinovaFSM::TransitionTable)/sizeof(KinovaFSM::Transition)) 
      {}

    ~StateMachine();

    //Initializes the StateMachine.
    void init();
    
    //Send Event to the StateMachine.
    void sendEvent(KinovaFSM::Event e);

    //Run the StateMachine.
    void process();

  private:
    KinovaFSM::Event inputEvent;
    State* currentState;
    int numberOfTransitions;

};
#endif
