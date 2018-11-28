#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "../thirdparty/libkindrv/include/kindrv.h"
#include "Event.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"

#define POSITION_RANGE 0.05


class KinovaArm {

  public:
	  KinovaArm() :
      Connected(false),
      Error(false),
      EventOut(KinovaFSM::NoEvent),
      Mode(KinovaStatus::NoMode),
      JoystickX(0),
      JoystickY(0),
      JoystickZ(0),
      JoystickCalcFactor(0.0025)
      {}
    ~KinovaArm();

    void error(const char* funcName, KinDrv::KinDrvException &e, bool warning);
    bool connect();
    void takeControl();
    void releaseControl();

    void dontMove();
    void checkInitialize();
    void initialize();
    void changeMode(KinovaStatus::SteeringMode nextMode);
    void modeChangeTimer();
    void move();
    void moveToPosition();

    void teachPosition(KinovaPts::Objective targetObjective);
    void moveToPoint();
    void savePoint();
    void nextPoint(); 

    void setJoystick(int x, int y, int z);

    void setTarget(KinovaPts::Objective targetObjective);

    bool getError();
    KinovaFSM::Event getEvent();
    void getPosition(float* coordinates);

  private:
    KinDrv::JacoArm *arm;
    PositionHandling PositionHandler;
    
    bool Connected;
    bool Error;
    KinovaStatus::SteeringMode Mode;

    KinovaPts::Objective TargetObjective;
    KinovaPts::Objective TeachTarget;
    bool NeedsInit;

    int JoystickX;
    int JoystickY;
    int JoystickZ;
    float JoystickCalcFactor;

    timespec TimerStart;
    int ModeChangeTimer;

    KinovaFSM::Event EventOut;

    
};

#endif
