#include <stdio.h>

#include "StateMachine.h"



StateMachine::~StateMachine() {}



void StateMachine::init(KinovaArm* jacoZED) {
  currentState = KinovaFSM::initState;
  currentState->init(jacoZED);
  currentState->entryAction();
}

void StateMachine::sendEvent(KinovaFSM::Event e) {
  inputEvent = e;
}


void StateMachine::process() {
 
  KinovaFSM::Event e = inputEvent;
  inputEvent = KinovaFSM::NoEvent;
  
  for (int i = 0; i<numberOfTransitions; i++) {
    if ( (currentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event) )  {
      KinovaFSM::TransitionTable[i].currentState->exitAction();
      currentState = KinovaFSM::TransitionTable[i].nextState;
      currentState->entryAction();
      break;
    }
  }
  currentState->tickAction();
}




