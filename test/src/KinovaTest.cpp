//
///***************************************************************************
// *  Kinova Main
// *
// *  Created: Fri Jun 15 2018
// ****************************************************************************/
//
//#include <stdio.h>
//#include <unistd.h>
//#include <poll.h>
////#include "StateMachine.h"
////#include "Transition.h"
//
//
//#include "CommandHandling.h"
//
////StateMachine *fsm;
////CommandHandling *communication;
//
// CommandHandling communication;
//
// void wait(int n) {
//  for (int i =0; i<n; i++) {
//    communication.process();
//    usleep(200000);
//  }
//}
//
// void move(int x, int y, int z, int n) {
//  printf("Move araound\n");
//  for (int i =0; i<n; i++) {
//    communication.debugSetJoystick(x,y,z);
//    communication.process();
//    usleep(200000);
//  }
//  communication.debugSetJoystick(0,0,0);
//  communication.process();
//  usleep(200000);
//}
//
//
// int main(int argc, char *argv[])
//{
//
//  printf("-------------------------\n");
//  printf("|      KinovaTest!      |\n");
//  printf("-------------------------\n");
//
//  /************************/
//  /* Testing StateMachine */
//  /************************/
//  /*
//  StateMachine fsm;
//  fsm.init();
//  int i;
//  printf("0 = No Event\n1 = Initialize\n2=Shutdown\n");
//  while (true) {
//
//    scanf("%d", &i);
//    if (i == 1) {
//      fsm.sendEvent(KinovaFSM::Initialize);
//      printf("Sent evInitialize\n");
//    }
//    else if(i == 2) {
//      fsm.sendEvent(KinovaFSM::Shutdown);
//      printf("Sent evShutdown\n");
//    }
//    fsm.process();
//    usleep(1000000);
//  }
//  */
//
//  /***************************/
//  /* Testing CommandHandling */
//  /***************************/
//  /*
//  communication.init();
//  communication.process();
//  usleep(200000);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(10);
//  communication.debugSendEvent(KinovaFSM::SetMode,3);
//  wait(10);
//  communication.debugSendEvent(KinovaFSM::SetMode,4);
//  wait(10);
//  communication.debugSendEvent(KinovaFSM::SetMode,2);
//  wait(10);
//  */
//
//  /********************/
//  /* Testing Steering */
//  /********************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::SetModeTranslation);
//  wait(5);
//  move(1000,0,0,5);
//  move(-1000,0,0,5);
//  move(0,1000,0,5);
//  move(0,-1000,0,5);
//  move(0,0,1000,5);
//  move(0,0,-1000,5);
//  */
//
//  /*********************/
//  /* Testing MoveToPos */
//  /*********************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,1);
//  wait(20);
//  */
//
//  /***********************/
//  /* Testing getPosition */
//  /***********************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(40);
//  communication.debugSendEvent(KinovaFSM::SetMode,1);
//  wait(5);
//  communication.debugPrintPosition();
//  wait(5);
//  */
//
//  /****************/
//  /* Testing Init */
//  /****************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(30);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,1);
//  wait(30);
//  */
//
//  /*******************************/
//  /* Testing TeachState Movement */
//  /*******************************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(30);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,1);
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::Teach,1);
//  wait(5);
//  move(0,500,0,5);
//  move(0,-500,0,5);
//  communication.debugSendEvent(KinovaFSM::SetMode,3);
//  wait(5);
//  move(0,500,0,5);
//  move(0,-500,0,5);
//  communication.debugSendEvent(KinovaFSM::Exit);
//  wait(5);
//  move(0,500,0,5);
//  move(0,-500,0,5);
//  */
//
//  /**********************/
//  /* Testing TeachState */
//  /**********************/
//  /*
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(30);
//  communication.debugSendEvent(KinovaFSM::SetMode,1);
//  //wait(20);
//  //communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go to Bell
//  //wait(40);
//  //communication.debugSendEvent(KinovaFSM::GoToPosition,1); //Go to Home
//  //wait(20);
//  communication.debugSendEvent(KinovaFSM::Teach,2);        //change to Teach Mode Bell:0
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go To Bell:0
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::Next);           //change to Teach Mode Bell:1
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go To Bell:1
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::Next);           //change to Teach Mode Bell:2
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go To Bell:2
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::Next);           //change to Teach Mode Bell:3
//  wait(5);
//  move(200,0,0,5);                                         //Move away.
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::SavePoint);      //Save current Position to Bell:3
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::Exit);
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,1); //Go to Home
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::GoToPosition,2); //Go to Bell
//  wait(20);
//  */
//
//  /***********************/
//  /* Testing Save Points */
//  /***********************/
//
//  communication.init();
//  wait(1);
//  communication.debugSendEvent(KinovaFSM::Initialize);
//  wait(30);
//  communication.debugSendEvent(KinovaFSM::SetMode,1);
//  wait(20);
//  communication.debugSendEvent(KinovaFSM::Teach,1);        //change to Teach Mode Home:0
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::SavePoint,0);    //Save Point
//  wait(5);
//  communication.debugSendEvent(KinovaFSM::Exit);           //Exit Teach Mode (Saves Data to File)
//  wait(5);
//}
//
//

#include "IntegrationSuite.h"
#include "KinovaArmSuite.h"
#include "MatrixSuite.h"
#include "PositionHandlingSuite.h"
#include "SequenceSuite.h"

#include <cute/cute.h>
#include <cute/cute_runner.h>
#include <cute/ide_listener.h>
#include <cute/xml_listener.h>

bool runAllTests(int argc, char const *argv[]) {
	cute::suite matrixSuite = make_suite_MatrixSuite();
	cute::suite positionHandlingSuite = make_suite_PositionHandlingSuite();
	cute::suite kinovaArmSuite = make_suite_KinovaArmSuite();
	cute::suite sequenceSuite = make_suite_SequenceSuite();
	cute::suite integrationSuite = make_suite_IntegrationSuite();
	cute::xml_file_opener xmlfile(argc, argv);
	cute::xml_listener<cute::ide_listener<>> lis(xmlfile.out);
	auto runner = cute::makeRunner(lis, argc, argv);
	bool success = runner(matrixSuite, "Matrix Suite Tests");
	success &= runner(kinovaArmSuite, "KinovaArm Suite Tests");
	success &= runner(positionHandlingSuite, "Position Handling Suite Tests");
	success &= runner(sequenceSuite, "Sequence Suite Tests");
	success &= runner(integrationSuite, "Integration Suite Tests");
	return success;
}

int main(int argc, char const *argv[]) {
	return runAllTests(argc, argv) ? EXIT_SUCCESS : EXIT_FAILURE;
}
