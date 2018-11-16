#include <stdio.h>

#include "StateMachine.h"



StateMachine::~StateMachine() {}



void StateMachine::init(KinovaArm* jacoZED) {
  CurrentState = KinovaFSM::initState;
  CurrentState->init(jacoZED);
  //CurrentState->entryAction();
}


void StateMachine::sendEvent(KinovaFSM::Event e) {
  InputEvent = e;
}

  
void StateMachine::process() {
  bool StateChange = false;
  KinovaFSM::Event e = InputEvent;
  InputEvent = KinovaFSM::NoEvent;

  //Print handled Event and reset InputEvent.
  if(e != KinovaFSM::NoEvent && e != KinovaFSM::Tick) {
    printf("StateMachine: Processing Event '%s'\n", KinovaFSM::EventName[e]);
  }
  for (int i = 0; i<NumberOfTransitions; i++) {
    if ( (CurrentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event) )  {
      KinovaFSM::TransitionTable[i].currentState->exitAction();
      CurrentState = KinovaFSM::TransitionTable[i].nextState;
      CurrentState->entryAction();
      StateChange = true;
      break;
    }
  }
  if(!StateChange)  {
    CurrentState->tickAction();
  }
}




