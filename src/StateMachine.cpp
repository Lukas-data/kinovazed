#include <stdio.h>
#include "StateMachine.h"


using namespace KinovaFSM;

StateMachine::~StateMachine() {}



void StateMachine::init() {
  currentState = initState;
  currentState->entryAction();
}

void StateMachine::sendEvent(Event e) {
  inputEvent = e;
}


void StateMachine::process() {
 
  Event e = inputEvent;
  inputEvent = NoEvent;
  
  for (int i = 0; i<numberOfTransitions; i++) {
    if ( (currentState == TransitionTable[i].currentState) && (e == TransitionTable[i].event) )  {
      TransitionTable[i].currentState->exitAction();
      currentState = TransitionTable[i].nextState;
      currentState->entryAction();
      break;
    }
  }
  currentState->tickAction();
}




