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
  bool StateChange = false;
  KinovaFSM::Event e = inputEvent;
  inputEvent = KinovaFSM::NoEvent;
  //Print handled Event and reset inputEvent.
  if(e != KinovaFSM::NoEvent && e != KinovaFSM::Tick) {
    printf("StateMachine: Processing Event '%s'\n", KinovaFSM::EventName[e]);
  }

  for (int i = 0; i<numberOfTransitions; i++) {
    if ( (currentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event) )  {
      KinovaFSM::TransitionTable[i].currentState->exitAction();
      currentState = KinovaFSM::TransitionTable[i].nextState;
      currentState->entryAction();
      StateChange = true;
      break;
    }
  }
  if(!StateChange)  {
    currentState->tickAction();
  }
}




