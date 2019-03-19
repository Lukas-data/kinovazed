#include <stdio.h>

#include "StateMachine.h"
#include "Log.h"



StateMachine::~StateMachine() {}



void StateMachine::init(KinovaArm* jacoZED) {
  CurrentState = KinovaFSM::initState;
  CurrentState->init(jacoZED);
}


void StateMachine::sendEvent(KinovaFSM::Event e) {
  InputEvent = e;
  InputVariable = 0;
}


void StateMachine::sendEvent(KinovaFSM::Event e, int eventVar) {
  InputEvent = e;
  InputVariable = eventVar;  
}


/*Processes InputEvent and InputVariable in Statemachine. Returns true if state Change is peformed.*/
bool StateMachine::process() {
  bool StateChange = false;
  KinovaFSM::Event e = InputEvent;
  InputEvent = KinovaFSM::NoEvent;

  int var = InputVariable;
  InputVariable = 0;

  //Print handled Event and reset InputEvent.
  for (int i = 0; i<NumberOfTransitions; i++) {
    if ( (CurrentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event) )  {
      ALL_LOG(logINFO) << "StateMachine: Processing Event '" << KinovaFSM::EventName[e] << "'";
      KinovaFSM::TransitionTable[i].currentState->exitAction();
      CurrentState = KinovaFSM::TransitionTable[i].nextState;
      CurrentState->setEventVar(var);
      CurrentState->entryAction();
      StateChange = true;
      return true;
    }      
  }
  ALL_LOG(logDEBUG4) << "StateMachine: Not Processing Event '" << KinovaFSM::EventName[e] << "'";
  CurrentState->tickAction(); 
  return false;
}



