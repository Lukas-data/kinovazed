#ifndef _KINOVAFUNCTION_H_
#define _KINOVAFUNCTION_H_

#include "../thirdparty/libkindrv/include/kindrv.h"
#include "Event.h"
#include "KinovaStatus.h"
#include "PositionHandling.h"

#define POSITION_RANGE 0.05
#define ROTATION_RANGE 0.25
#define VELOCITY_RANGE 0.000002

class KinovaArm {

  public:
	  KinovaArm() :
      Connected(false),
      Error(false),
      Initialized(false),
      Active(false),
      ExternalEvent(KinovaFSM::NoEvent),
      InternalEvent(KinovaFSM::NoEvent),
      currentPosition(-1),
      Mode(KinovaStatus::NoMode),
      JoystickX(0),
      JoystickY(0),
      JoystickZ(0),
      JoystickCalcFactor(0.0025)
      {}
    ~KinovaArm();

    bool connect();
    void disconnect();
    void reconnectOnError();
    void takeControl();
    void releaseControl();

    void dontMove();
    void checkInitialize();
    void initialize();
    void changeMode(KinovaStatus::SteeringMode nextMode);
    void modeChangeTimer();
    void move();
    void moveToPosition(bool init);
    void sequenceDone();

    void teachPosition(KinovaPts::Objective targetObjective);
    void moveToPoint();
    void moveToOrigin();
    void savePoint(int EventVariable);
    void saveOrigin();
    void deletePoint();
    void previousPoint(int EventVariable); 
    void nextPoint(int EventVariable); 

    void setJoystick(int x, int y, int z);

    void setTarget(KinovaPts::Objective targetObjective);

    bool getError();
    bool getInitialize();
    bool getActive();
    int  getMode();
    int  getCurrentPosition();
    int  getCurrentPoint();

    KinovaFSM::Event getExternalEvent();
    KinovaFSM::Event getInternalEvent();
    void getPosition(float* coordinates);

    void setActive();
    void setInactive();

  private:
    KinDrv::JacoArm *arm;
    PositionHandling PositionHandler;
    
    bool Connected;
    bool Error;
    bool Initialized;
    bool Active;
    KinovaStatus::SteeringMode Mode;
    int currentPosition;

    KinovaPts::Objective TargetObjective;
    KinovaPts::Objective TeachTarget;
    bool NeedsInit;

    int JoystickX;
    int JoystickY;
    int JoystickZ;
    float JoystickCalcFactor;

    timespec ModeChangeTimerStart;
    int ModeChangeTimer;

    timespec MoveTimerStart;

    KinovaFSM::Event ExternalEvent;
    KinovaFSM::Event InternalEvent;

    float LastCoordinates[6];

    int PointReachedCount;

    void error(const char* funcName, KinDrv::KinDrvException &e, bool warning);
    void error(const char* funcName, const char* errorMsg);
    bool checkIfReached(float* targetCoordinates, float* currentCoordinates);
    bool checkCurrents();
    void getForces();
};

#endif
