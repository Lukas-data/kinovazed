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
    
    //Debug only:
    void debugSendEvent(KinovaFSM::Event e);
    void debugSendEvent(KinovaFSM::Event e, int var);
    void debugSetJoystick(int x, int y, int z);
    void debugPrintPosition();

  private:
    KinovaArm JacoZED;
    TCPServer RoboRio;
    StateMachine KinovaSM;

    KinovaFSM::Event CommandOut;
    KinovaFSM::Event CommandIn;
    int CommandVarOut;
    int CommandVarIn;
    int DataOut[3];
    int DataIn[3];

    void getInputs();
    void sendOutputs(int event, int eventVar);
    void checkInputEvent(KinovaFSM::Event &event, int &eventVar);
    bool checkOutputEvent(KinovaFSM::Event &event, int &eventVar);
};
#endif
