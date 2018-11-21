#ifndef _COMMANDHANDLING_H_
#define _COMMANDHANDLING_H_

#include "StateMachine.h"
#include "TCPServer.h"
#include "KinovaArm.h"
#include "KinovaStatus.h"


/**/




class CommandHandling {
  public:
    void init();
    void process();
    void sendCommand(Command::Name command);

    //Debug only:
    void debugSendEvent(KinovaFSM::Event e);
    void debugSendEvent(KinovaFSM::Event e, int var);
    void debugSetJoystick(int x, int y, int z);
    void debugPrintPosition();

  private:
    KinovaArm JacoZED;
    TCPServer RoboRio;
    StateMachine KinovaSM;

    Command::Name commandOut;
    Command::Name commandIn;
    int dataOut[3];
    int dataIn[3];
    int modeOut;
    int modeIn;
};
#endif
