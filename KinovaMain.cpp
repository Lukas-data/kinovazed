
/***************************************************************************
 *  Kinova Main
 *
 *  Created: Fri Jun 15 2018
 ****************************************************************************/


#include <stdio.h>
#include "KinovaArm.h"
#include "TCPServer.h"


TCPServer *RoboRio;
KinovaArm *ZEDArm;

int command;
int data[3];

bool responseSent;

enum Commands { CLOSE_SERVER = -2,
                CLOSE_CONNECTION = -1,
                NOTHING = 0,
                INIT = 1,
                SET_MODE = 2,
                MOVE = 3,
                TO_POS = 4,
                POS_REACHED = 5,
                ESTOP = 91,
                ESTOP_QUIT = 92,
                ERROR = 99};


void getCommandAndData()
{
  command = RoboRio->getCommand();
  for(int n = 0; n<3; n++) {
    data[n] = RoboRio->getData(n);
  }
}


void executeCommand() {
  getCommandAndData();
  int ArmMode;
  if (command != 0) {
    printf("Recieving Command: %d ( %d, %d, %d)\n",command, data[0], data[1], data[2]);
  }
  switch (command) {
    case NOTHING  : //printf("Nothing at all!\n");
                    break;
    case INIT     : ZEDArm->setDoInitialize();
                    break;
    case SET_MODE : ZEDArm->DoSetMode(data[0]);
                    break;
    case MOVE     : ZEDArm->DoSetSpeed(data[0], data[1], data[2]);
                    break;
    case TO_POS   : ZEDArm->DoMoveToPos(data[0]);
                    break;
    case ESTOP    : ZEDArm->DoEmergencyStop();
                    break;
    case ESTOP_QUIT: ZEDArm->DoQuitEmergencyStop();
                     break;
    case ERROR    : //TODO: ERROR-Handling
                    printf("Error in Communication!\n");
                    break;
    default       : printf("UNKNOWN COMMAND: %d\n",command);
  }
}


void sendAnswer() {
  command = ZEDArm->getDataToSend();
  for(int i = 0; i<3;i++) {data[i] = 0;}
  switch (command) {
    case NOTHING  : break;
    case INIT     : break;
    case SET_MODE : data[0] = ZEDArm->getCurrentMode();
                    break;
    case ERROR    : //TODO: ERROR-Handling
                    break;
    case ESTOP    : break;
    case ESTOP_QUIT: break;
    default       : printf("UNKNOWN COMMAND: %d\n",command);
  }
  if (command != 0) {
    printf("Sending Response: %d ( %d, %d, %d)\n",command, data[0], data[1], data[2]);
  }
  RoboRio->sendTCP(command, data[0], data[1], data[2]);

}




int main(int argc, char *argv[])
{
  bool ArmConnected;
  bool ServerCreated;

  while (1) {
    printf("\n");
    printf("/-------------------------\\\n");
    printf("| KinovaCommunicationMain | \n");
    printf("\\-------------------------/\n\n");
  
    while (ArmConnected == false) {
      ArmConnected = true;
      try{
        ZEDArm = new KinovaArm();
      }
      catch( const std::runtime_error& e ) {
        printf("kill");
        ArmConnected = false;
        return -1;
      }
    }

    while (ServerCreated == false) {
      ServerCreated = true;
      try {
        RoboRio = new TCPServer();
      }
      catch( const std::runtime_error& e ) {
        printf("kill");
        ServerCreated = false;
      }
    }

    
    while (1) {
      RoboRio->waitForClient();

      while (1) {
        executeCommand();
        
        if (command < 0)
          break;
        try {
          ZEDArm->JacoMain();
        }
        catch (const std::runtime_error& e) {
        printf("I caught something!");
        }
        sendAnswer();

        //ZEDArm->getArmStatus(); Zu langsam
        
        //usleep(10000);
        
      }
      if(RoboRio->closeOnCommand(command) < 0) {
        delete RoboRio;
        break;
      } 
    }

    printf("\n");
  }
}
