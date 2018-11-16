#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "../thirdparty/libkindrv/include/kindrv.h"
#include "Event.h"
#include "KinovaStatus.h"

#define POSITION_RANGE 0.05

namespace KinovaStatus{

}

class KinovaArm {
  public:
	  KinovaArm() :
      Connected(false),
      Error(false),
      EventOut(KinovaFSM::NoEvent),
      Mode(KinovaStatus::NoMode) 
      {}
  //calcFactor = 0.0025;
  //EmergencyStop = false;
    ~KinovaArm(); 
    void error(const char* funcName, KinDrv::KinDrvException &e, bool warning);
    bool connect();
    void takeControl();
    void releaseControl();

    void dontMove();
    void initialize();
    void changeMode(KinovaStatus::SteeringMode nextMode);

    void setNextMode(KinovaStatus::SteeringMode mode);
    void resetNextMode();

    bool getError();
    KinovaFSM::Event getEvent();

    

  private:
    KinDrv::JacoArm *arm;
    
    bool Connected;
    bool Error;
    KinovaStatus::SteeringMode Mode;
    
    KinovaFSM::Event EventOut;
    
    








/*---------------------

    void test(int a);
    void JacoMain();
    int goto_retract();
    int goto_home();
    int setJSMode(int state);
    void moveArm();
        
    int getCurrentSpeedX();
    int getCurrentSpeedY();
    int getCurrentSpeedZ();
    int getDataToSend();
    int getCurrentMode();
    void getArmStatus();

    void setDoInitialize();
    void DoSetMode(int mode);
    void DoSetSpeed(int x, int y, int z);
    void DoEmergencyStop();
    void DoQuitEmergencyStop();
    void DoMoveToPos(int pos);


  private:
    enum Data { NOTHING = 0,
                INIT = 1,
                MODE_SET = 2,
                ESTOP = 91,
                ESTOP_QUIT = 92,
                ERROR = 99
              };
    enum Positions {  POS_ZEDHOME = 0,
                      POS_NOPOS = 99
              };

    

    int testInt;

    
    float currentPositionCord[6];
    int currentPosition;
    float requestedPositionCord[6];
    int requestedPosition;

    int currentRobotMode;
    int requestedRobotMode;
    int currentJSMode;
    int requestedJSMode;
    int currentKinematicsMode;
    int requestedKinematicsMode;
    jaco_joystick_axis_t axes;
    
    int movingSequenceNo;

    bool Initializing;
    bool Ready;
    bool MovingToPosition;
    bool SettingMode;
    bool MovingHome;

    bool DoInitialize;

    int setModeCycleCount;

    double currentSpeedX;
    double currentSpeedY;
    double currentSpeedZ;
    bool EmergencyStop;
    float calcFactor;


    int DataBuffer[8];

    void initialize();
    void setMode();
    void writeDataToBuffer(int Data);
    int getDataFromBuffer();
    void clearBuffer();
    void stopArm();
    void moveToPos();
*/
};

#endif
