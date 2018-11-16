#ifndef _COMMANDHANDLING_H_
#define _COMMANDHANDLING_H_

#include "StateMachine.h"
#include "TCPServer.h"
#include "KinovaArm.h"
#include "KinovaStatus.h"

/**/

struct Command {
  enum Name {
    Nothing = 0,
    Init = 1,
    SetMode = 2,
    Steer = 3,
    MoveToPos = 4,
    PosReached = 5,
    EStop = 91,
    EStop_Quit = 92,
    Error = 99
  };
};


class CommandHandling {
  public:
    void init();
    void process();
    void sendCommand(Command::Name command);

    //Debug only:
    void debugSendEvent(KinovaFSM::Event e);

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
