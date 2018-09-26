#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include <libkindrv/kindrv.h>

#define POSITION_RANGE 0.05

using namespace KinDrv;

class KinovaArm {
	
  private:
    enum Data { NOTHING = 0,
                INIT = 1,
                MODE_SET = 2,
                ESTOP = 91,
                ESTOP_QUIT = 92,
                ERROR = 99
              };

    JacoArm *arm;

    jaco_retract_mode_t Status;
    float currentPosition[6];
    float requestedPosition[6];

    int currentRobotMode;
    int requestedRobotMode;
    int currentJSMode;
    int requestedJSMode;
    int currentKinematicsMode;
    int requestedKinematicsMode;
    jaco_joystick_axis_t axes;

    bool Initializing;
    bool Ready;
    bool MovingToPosition;
    bool SettingMode;

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
    


  public:
	  KinovaArm();
    ~KinovaArm(); 
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
    

    

};

#endif
