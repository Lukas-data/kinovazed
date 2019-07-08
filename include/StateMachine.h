#ifndef _STATEMACHINE_H_
#define _STATEMACHINE_H_

#include "Event.h"
#include "KinovaArm.h"
#include "State.h"
#include "Transition.h"


/*This State Machine runs the entry, exit and tick events from the states in "State.h" according to the events and transitions in "Transition.h". There is no Queue. If an event finds no use, it is dropped.*/

#define LOOPTIME 50.0

class StateMachine {

  public:
    StateMachine() :
      //Calculates the Number Of Transitions in the Transition Table as it is assumed unchanged during runtime.
      InputEvent(KinovaFSM::NoEvent),
      NumberOfTransitions(sizeof(KinovaFSM::TransitionTable)/sizeof(KinovaFSM::Transition))
      {}

    ~StateMachine();

    //Initializes the StateMachine.
    void init(KinovaArm* jacoZED);
    
    //Send Event to the StateMachine.
    void sendEvent(KinovaFSM::Event e);
    void sendEvent(KinovaFSM::Event e, int eventVar);


    //Run the StateMachine. Return True if StateChange is performed.
    bool process();
    
    //returns current Event of StateMachine
    //int getCurrentState();

  private:
    KinovaFSM::Event InputEvent;
    int InputVariable;
    State* CurrentState;
    int NumberOfTransitions;

    timespec LastTick;


};
#endif

