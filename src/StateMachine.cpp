#include "StateMachine.h"
#include "Log.h"

#include <cstdio>
#include <ctime>
#include <memory>


StateMachine::StateMachine(std::shared_ptr<KinovaArm> jacoZed) : CurrentState{KinovaFSM::initState} {
	CurrentState->init(jacoZed);
	clock_gettime(CLOCK_REALTIME, &LastTick);
}

StateMachine::~StateMachine() {}


/*Processes InputEvent and InputVariable in Statemachine. Returns true if state Change is peformed.*/
bool StateMachine::process(KinovaFSM::Event e, int var) {
  
  //Print handled Event and reset InputEvent.
  for (int i = 0; i<NumberOfTransitions; i++) {
    if ( (CurrentState == KinovaFSM::TransitionTable[i].currentState) && (e == KinovaFSM::TransitionTable[i].event) )  {
      ALL_LOG(logINFO) << "StateMachine: Processing Event '" << KinovaFSM::eventNames[e] << "'";
      KinovaFSM::TransitionTable[i].currentState->exitAction();
      CurrentState = KinovaFSM::TransitionTable[i].nextState;
      CurrentState->setEventVar(var);
      CurrentState->entryAction();
      return true;
    }      
  }
  ALL_LOG(logDEBUG4) << "StateMachine: Not Processing Event '" << KinovaFSM::eventNames[e] << "'";
  timespec timeNow;
  clock_gettime(CLOCK_REALTIME, &timeNow);
  double elapsedTime = (timeNow.tv_sec-LastTick.tv_sec) * 1000.0 +
                    (timeNow.tv_nsec-LastTick.tv_nsec) / 1000000.0;
  if( elapsedTime > looptime ) {
    ALL_LOG(logDEBUG4) << "Tick!";
    CurrentState->tickAction();
    clock_gettime(CLOCK_REALTIME, &LastTick);
  }
  
  return false;
}



