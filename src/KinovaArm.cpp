#include <stdio.h>
#include <ctime> 

#include "KinovaArm.h"



#define KINOVA_DUMMY false

KinovaArm::~KinovaArm() {
  if (KINOVA_DUMMY == false) {   
    delete arm;
    printf("Connection to JacoArm closed \n");
  }
  Connected = false;
}

/*Sets Errorflag and removes Connected flag.*/
void KinovaArm::error(const char* funcName, KinDrv::KinDrvException &e, bool warning) {
  printf("KinovaArm::%s(): Error %i, %s\n",funcName, e.error(), e.what());
  if (!warning) {
    Connected = false; //Simplification!!
    Error = true;
  }
}


/*Tries to connect to KinovaArm*/
bool KinovaArm::connect() {
  if (KINOVA_DUMMY == false && Connected == false) {
    try {
      arm = new KinDrv::JacoArm();
      Connected = true;
      return 1;
    } catch( KinDrv::KinDrvException &e ) {
      error("connect", e, true);
      return 0;
    }
  }
  else {
    Connected = true;
    return 1;
  }
}


/*Gain API Control*/
void KinovaArm::takeControl() {
  if (KINOVA_DUMMY == false) {
    try {
      arm->start_api_ctrl();
      printf("Gained API control over the arm.\n");
      //armStatus = arm->get_status();
      //printf("Arm is currently in state: %i \n", armStatus);
    }
    catch( KinDrv::KinDrvException &e ) {
      //printf("KinovaArm::takeControl(): Error %i, %s\n", e.error(), e.what());
      error("takeControl", e, false);
    }
  }
}


/*Release API Control*/
void KinovaArm::releaseControl() {
  if (KINOVA_DUMMY == false) {
    try {
      arm->stop_api_ctrl();
      printf("Released API control over the arm.\n");
      //armStatus = arm->get_status();
      //printf("Arm is currently in state: %i \n", armStatus);
    }
    catch( KinDrv::KinDrvException &e ) {
      //printf("KinovaArm::takeControl(): Error %i, %s\n", e.error(), e.what());
      error("releaseControl", e, false);
    }
  }
}


/*Stops arm*/
void KinovaArm::dontMove() {
  if (KINOVA_DUMMY == false) {
    KinDrv::jaco_joystick_axis_t axes;
    axes.trans_lr  = 0; 
    axes.trans_fb  = 0;
    axes.trans_rot = 0;
    axes.wrist_lr  = 0;
    axes.wrist_fb  = 0;
    axes.wrist_rot = 0;
    try {
      arm->move_joystick_axis(axes);
      arm->release_joystick();
    }
    catch( KinDrv::KinDrvException &e ) {
      error("dontMove", e, false);
    }
  }
}

void KinovaArm::checkInitialize() {
  PositionHandler.init();
  if (KINOVA_DUMMY == false) {
    try {
      KinDrv::jaco_retract_mode_t armStatus = arm->get_status();

      //Moves Arm to Kinova Home position if not in initialized Position
      if( armStatus == KinDrv::MODE_NOINIT) {
        setTarget(KinovaPts::Home);
      }
      else {
        EventOut = KinovaFSM::Initialized;
      }
    }
    catch ( KinDrv::KinDrvException &e ) {
      error("initialize", e, false);
    }
  }
  else {
    EventOut = KinovaFSM::Initialized;
    printf("Arm connection is turned off. Dummy-Initialized.\n");
  }
}


/*Initialize Arm and go to Home position*/
void KinovaArm::initialize()
{
  if (KINOVA_DUMMY == false) {
    try {
      KinDrv::jaco_retract_mode_t armStatus = arm->get_status();

      //Moves Arm to Kinova Home position if not in initialized Position
      if( armStatus == KinDrv::MODE_NOINIT) {
        //printf("Initializing: Status is NOINIT.\n");
        //push the "HOME/RETRACT" button until arm is initialized
        KinDrv::jaco_joystick_button_t buttons = {0};
        buttons[2] = 1;
        arm->push_joystick_button(buttons);
      }
      else {
        //printf("Initializing: Statuts is %d.\n",armStatus);
        arm->release_joystick();
        moveToPosition();
      }
    }
    catch ( KinDrv::KinDrvException &e ) {
      error("initialize", e, false);
    }
  }
  else {
    EventOut = KinovaFSM::Initialized;
    printf("Arm connection is turned off. Dummy-Initialized.\n");
  }
}


/*Changes current Mode and sets TimerTime (only necessary if kinematic 
mode on Jaco needs to be changed.). Default nextMode = NoMode is Translation!*/
void KinovaArm::changeMode(KinovaStatus::SteeringMode nextMode) {
  if (Mode != nextMode || Mode == KinovaStatus::NoMode) {
    if (KINOVA_DUMMY == false) {
      try {
        if( (Mode != KinovaStatus::Axis1 && Mode != KinovaStatus::Axis2)
         && (nextMode == KinovaStatus::Axis1 || nextMode == KinovaStatus::Axis2) ) {
          arm->set_control_ang();
          ModeChangeTimer=500;
        }
        if( (Mode != KinovaStatus::Translation && Mode != KinovaStatus::Rotation)
         && (nextMode == KinovaStatus::Translation || nextMode == KinovaStatus::Rotation
          || nextMode == KinovaStatus::NoMode) ) {
          arm->set_control_cart();
          ModeChangeTimer=500;
        }
      }
      catch( KinDrv::KinDrvException &e ) {
        error("changeMode", e, false);
      }
    }
    if (nextMode > 0 && nextMode <= 4) {
      Mode = nextMode;
    }
    else if (nextMode == 0) {
      Mode = KinovaStatus::Translation;
    }
    else {
      printf("KinovaArm: Requested Mode invalid.\n");
    } 
  }
  clock_gettime(CLOCK_REALTIME, &TimerStart);
}


/*Runs Timer and sends ModeChange Event when done.*/
void KinovaArm::modeChangeTimer() {
  timespec TimerNow;
  clock_gettime(CLOCK_REALTIME, &TimerNow);
  double elapsedTime = (TimerNow.tv_sec-TimerStart.tv_sec) * 1000 +
                    (TimerNow.tv_nsec-TimerStart.tv_nsec) / 1000000;
  if (elapsedTime > ModeChangeTimer) {
    EventOut = KinovaFSM::ModeSet;
    ModeChangeTimer=0;
  }
}


/*Moves arm according to Joystick signal.*/
void KinovaArm::move() {
  double currentSpeedX = -JoystickX*JoystickCalcFactor;
  double currentSpeedY = -JoystickY*JoystickCalcFactor;
  double currentSpeedZ = JoystickZ*JoystickCalcFactor;
  if (KINOVA_DUMMY == false) {
    KinDrv::jaco_joystick_axis_t axes;
    axes.trans_lr  = 0; 
    axes.trans_fb  = 0;
    axes.trans_rot = 0;
    axes.wrist_lr  = 0;
    axes.wrist_fb  = 0;
    axes.wrist_rot = 0;

    if (Mode == KinovaStatus::Translation || Mode == KinovaStatus::Axis1) {
      axes.trans_lr = currentSpeedX; 
      axes.trans_fb = currentSpeedY;
      axes.trans_rot = currentSpeedZ;
      //printf("MoveArm In Mode: 1 -> ");
    }
    if (Mode == KinovaStatus::Rotation || Mode == KinovaStatus::Axis2) {
      axes.wrist_lr = currentSpeedX;
      axes.wrist_fb = currentSpeedY;
      axes.wrist_rot = currentSpeedZ;
      //printf("MoveArm In Mode: 2 -> ");
    }
    try {
      arm->move_joystick_axis(axes);
      //printf("x= %f, y= %f, z= %f\n", currentSpeedX,currentSpeedY,currentSpeedZ);
    }
    catch( KinDrv::KinDrvException &e ) {
      error("move", e, false);
    }
  }
}


/*Sets the Targetpoint for the movement*/
void KinovaArm::setTarget(KinovaPts::Objective targetObjective) {
  if (targetObjective > 0 && targetObjective <= KinovaPts::NumberOfObjectives) {
    TargetObjective = targetObjective;
    PositionHandler.resetSequence();
  }
  else {
    printf("KinovaArm: invalid targetPosition.\n");
    EventOut = KinovaFSM::PositionReached;
  }
}


/*moves the Arm towards the next Point in the sequenceList.
If Point is reached, sequence is counted up.
If end of sequence is reached, PositionReached event is sent.*/
void KinovaArm::moveToPosition() {
  float targetCoordinates[6];
  float currentCoordinates[6];

  getPosition(currentCoordinates);
  //Check if Sequence is still going
  if ( PositionHandler.getCoordinates(targetCoordinates, TargetObjective) ) {
    //Check if in range
    bool PointReached = true;
    for (int i = 0; i<6; i++) {
      if (currentCoordinates[i] > ( targetCoordinates[i] + POSITION_RANGE ) ||
          currentCoordinates[i] < ( targetCoordinates[i] - POSITION_RANGE ) ) {
        PointReached = false;
      }
    }
    
    if (PointReached == true) {
      //Next Point in Sequence.
      PositionHandler.countSequence();
    }
    else {
      //Move to Position
      float fingers[3];
      try {
        arm->set_target_cart(targetCoordinates,fingers);
      }
      catch( KinDrv::KinDrvException &e ) {
        error("moveToPosition", e, false);
      }
    }
  }
  else {
    PositionHandler.resetSequence();
    EventOut = KinovaFSM::PositionReached;
  }
}

void KinovaArm::teachPosition(KinovaPts::Objective targetObjective) {
  if (targetObjective != 0) {
    if (targetObjective > 0 && targetObjective <= KinovaPts::NumberOfObjectives) {
      printf("new teachTarget, sequence reset.\n");
      TeachTarget = targetObjective;
      PositionHandler.resetSequence();
    }
    else {
      printf("KinovaArm: invalid teach Position.\n");
    }
  }
  printf("TeachMode: Teaching at Point %d:%d\n", TeachTarget, PositionHandler.getSequence());
}

void KinovaArm::moveToPoint() {
  float targetCoordinates[6];
  float currentCoordinates[6];

  getPosition(currentCoordinates);

  //Check if Sequence is still going
  if ( PositionHandler.getCoordinates(targetCoordinates, TeachTarget) ) {
    //Check if in range
    bool PointReached = true;
    for (int i = 0; i<6; i++) {
      if (currentCoordinates[i] > ( targetCoordinates[i] + POSITION_RANGE ) ||
          currentCoordinates[i] < ( targetCoordinates[i] - POSITION_RANGE ) ) {
        PointReached = false;
      }
    }
    
    if (PointReached == true) {
      EventOut = KinovaFSM::PositionReached;
    }
    else {
      //Move to Point
      float fingers[3];
      try {
       arm->set_target_cart(targetCoordinates,fingers);
      }
      catch( KinDrv::KinDrvException &e ) {
        error("moveToPoint", e, false);
      }
    }
  }
  else {
    printf("No further Points in Sequence.\n");
  }
}

/**/
void KinovaArm::savePoint() {
  float currentCoordinates[6];
  getPosition(currentCoordinates);

  PositionHandler.savePoint(currentCoordinates, TeachTarget);
  PositionHandler.writeToFile();
  EventOut = KinovaFSM::PointSaved;
}

void KinovaArm::nextPoint() {
  PositionHandler.countSequence();
  EventOut = KinovaFSM::NextPointSet;
}


void KinovaArm::setJoystick(int x, int y, int z) {
  //printf("KinovaArm::setJoystick (%d,%d,%d)\n",x,y,z);
  JoystickX = x;
  JoystickY = y;
  JoystickZ = z;
}

/*Get-Functions*/
bool KinovaArm::getError() { return Error; }

KinovaFSM::Event KinovaArm::getEvent() {
  KinovaFSM::Event e = EventOut;
  EventOut = KinovaFSM::NoEvent;
  return e;
}

void KinovaArm::getPosition(float* coordinates) {
  if (KINOVA_DUMMY == false) {
    KinDrv::jaco_position_t Position;
    try {
      Position = arm->get_cart_pos();
    }
    catch( KinDrv::KinDrvException &e ) {
        error("getPosition", e, false);
    }

    //transform jaco_position_t to float[]
    for (int i = 0; i<6; i++) {
      if (i<3) 
        coordinates[i] = Position.position[i];
      else
        coordinates[i] = Position.rotation[i-3];
    }
  }
  printf("currentCoordinates: (%f,%f,%f,%f,%f,%f)\n", coordinates[0],coordinates[1],coordinates[2],coordinates[3],coordinates[4],coordinates[5]);
}







