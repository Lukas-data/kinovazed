#include <stdio.h>
#include <ctime> 
#include <Log.h>
#include <string>

#include "KinovaArm.h"



#define KINOVA_DUMMY false

KinovaArm::~KinovaArm() {
  disconnect();
}

/*Prints Error/Warning-Message.Sets Errorflag and removes Connected flag on Error.*/
void KinovaArm::error(const char* funcName, KinDrv::KinDrvException &e, bool warning) {
  if (!warning) {
    ALL_LOG(logERROR) << "KinovaArm::" << funcName << "(): " << e.error()
                                                   << ", " << e.what();
    Connected = false; //Simplification!!
    Error = true;
  }
  else {
    ALL_LOG(logWARNING) << "KinovaArm::" << funcName << "(): " << e.error()
                                                     << ", " << e.what();
  }
}
void KinovaArm::error(const char* funcName, const char* errorMsg) {
  ALL_LOG(logERROR) << "KinovaArm::" << funcName << "(): " << errorMsg;
  Connected = false; //Simplification!!
  Error = true;
}



/*Tries to connect to KinovaArm*/
bool KinovaArm::connect() {
  if (KINOVA_DUMMY == false && Connected == false) {
    try {
      //KinovaArm::disconnect();
      arm = new KinDrv::JacoArm();
      Connected = true;
      ALL_LOG(logINFO) << "Connection to JacoArm established.";
      return 1;
    } catch( KinDrv::KinDrvException &e ) {
      error("connect", e, false);
      return 0;
    }
  }
  else {
    Connected = true;
    return 1;
  }
}


/*Disconnects to KinovaArm*/
void KinovaArm::disconnect() {
  if (KINOVA_DUMMY == false) {
    try {
      if( arm != NULL ) {
        delete arm;
      }
      ALL_LOG(logINFO) << "Connection to JacoArm closed. " ;
    } catch( KinDrv::KinDrvException &e ) {
      error("disconnect", e, false);
    }
  }
  Connected = false;
}


void KinovaArm::reconnectOnError() {
  if ( Error ) {
    ALL_LOG(logDEBUG3) << "Connection to JacoArm reopend after Error." ;
    if ( Connected ) { disconnect(); }
    Error = !connect();
  }
}

/*Gain API Control*/
void KinovaArm::takeControl() {
  if (KINOVA_DUMMY == false) {
    try {
      arm->start_api_ctrl();
      ALL_LOG(logINFO) << "Gained API control over the arm." ;
    }
    catch( KinDrv::KinDrvException &e ) {
      error("takeControl", e, false);
    }
  }
}


/*Release API Control*/
void KinovaArm::releaseControl() {
  if (KINOVA_DUMMY == false) {
    try {
      arm->stop_api_ctrl();
      ALL_LOG(logINFO) << "Released API control over the arm.";
      Error = false;
    }
    catch( KinDrv::KinDrvException &e ) {
      error("releaseControl", e, false);
    }
  }
  Initialized = false;
}


/*Stops arm and erases all trajectories.*/
void KinovaArm::dontMove() {
  if (KINOVA_DUMMY == false) {
    //reset Movement-Target
    try {
      arm->erase_trajectories();
    }
    catch( KinDrv::KinDrvException &e ) {
      error("dontMove: stopping Movement", e, false);
    }
    //reset Joystick-input
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
      error("dontMove: releasing Joystick", e, false);
    }
  }
}


/*Checks if Arm is allready initialized and sets Home as targetPosition if not.*/
void KinovaArm::checkInitialize() {
  Initialized = false;
  PositionHandler.init();
  if (KINOVA_DUMMY == false) {
    try {
      KinDrv::jaco_retract_mode_t armStatus = arm->get_status();

      //Moves Arm to Kinova Home position if not in initialized Position
      if( armStatus == KinDrv::MODE_NOINIT) {
        setTarget(KinovaPts::Home);
      }
      else {
        ExternalEvent = KinovaFSM::Initialized;
        Initialized = true;
      }
    }
    catch ( KinDrv::KinDrvException &e ) {
      error("initialize", e, false);
    }
  }
  else {
    ExternalEvent = KinovaFSM::Initialized;
    ALL_LOG(logINFO) << "Arm connection is turned off. Dummy-Initialized." ;
  }
}


/*Initialize Arm and go to Home position*/
void KinovaArm::initialize()
{
  currentPosition = 0;
  if (KINOVA_DUMMY == false) {
    if (!Initialized) {
      try {
        KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
        //Moves Arm to Kinova Home position if not in initialized Position
        if( armStatus == KinDrv::MODE_NOINIT) {
          ALL_LOG(logDEBUG3) << "Initializing: Status is NOINIT.";
          //push the "HOME/RETRACT" button until arm is initialized
          KinDrv::jaco_joystick_button_t buttons = {0};
          buttons[2] = 1;
          arm->push_joystick_button(buttons);
        }
        else {
          ALL_LOG(logDEBUG3) << "Initializing: Statuts is " << armStatus << ".";
          arm->release_joystick();
          moveToPosition(true);
        }
      }
      catch ( KinDrv::KinDrvException &e ) {
        error("initialize", e, false);
      }
    }
  }
  else {
    Initialized = true;
    ExternalEvent = KinovaFSM::Initialized;
    ALL_LOG(logINFO) << "Arm connection is turned off. Dummy-Initialized.";
  }
}

/*Pushes Button if Arm is not allready retracted*/
void KinovaArm::startRetracting() {
  currentPosition = 0;
  if(!KINOVA_DUMMY) {
    Homed = false;
    try {
      KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
      if (armStatus != 3) {
        ALL_LOG(logDEBUG) << "Arm starts retracting from Mode: " << armStatus;
        arm->push_joystick_button(2);
      }
      else {
        arm->release_joystick();
        ExternalEvent = KinovaFSM::Retracted;
      }
    }
    catch ( KinDrv::KinDrvException &e ) {
      error("retract", e, false);
    }
  }
}

/*brings arm to retracted position*/
void KinovaArm::retract() {
  currentPosition = 0;
  if(!KINOVA_DUMMY) {
    try {
      KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
      if (armStatus != 3) {
        ALL_LOG(logDEBUG3) << "Retracting Arm. Currently in Mode: " << armStatus;
      }
      if (armStatus == KinDrv::MODE_READY_STANDBY) {  
        Homed = true;
      }
      else if (Homed || armStatus == KinDrv::MODE_RETRACT_TO_READY) {
        arm->release_joystick();
        arm->push_joystick_button(2);
        Homed = false;
      }
      else if (armStatus == KinDrv::MODE_RETRACT_STANDBY) {
          arm->release_joystick();
          ExternalEvent = KinovaFSM::Retracted;
      }
      else if (armStatus == KinDrv::MODE_ERROR) {
        error("retract", "Arm has an Error!");
      }
    }
    catch ( KinDrv::KinDrvException &e ) {
      error("retract", e, false);
    }
  }
}


/*Moves Arm out of retracted Position to own home position.*/
void KinovaArm::unfold() {
  if(!KINOVA_DUMMY) {
    if (currentPosition != KinovaPts::Home) {
      if (TargetObjective != KinovaPts::Home) {
        setTarget(KinovaPts::Home);
      }
      try {
        KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
        //if (armStatus != 3) {
        ALL_LOG(logDEBUG4) << "unfolding from Mode: " << armStatus;
        //}
        switch (armStatus) {    
          case KinDrv::MODE_RETRACT_TO_READY:
            arm->push_joystick_button(2);
            break;
          case KinDrv::MODE_READY_TO_RETRACT:
            arm->push_joystick_button(2);
          case KinDrv::MODE_RETRACT_STANDBY:
            arm->release_joystick();
            arm->push_joystick_button(2);
            break;
          case KinDrv::MODE_NORMAL_TO_READY:
          case KinDrv::MODE_NOINIT:
            arm->push_joystick_button(2);
            break;
          case KinDrv::MODE_ERROR:
            error("unfold", "Arm has an Error!");
            break;
          case KinDrv::MODE_NORMAL:
          case KinDrv::MODE_READY_STANDBY:
            arm->release_joystick();
            moveToPosition(true);
            break;
        }
      }
      catch ( KinDrv::KinDrvException &e ) {
        error("unfold", e, false);
      }
    }
    else {
      ExternalEvent = KinovaFSM::Unfolded;
      
    }
  }
  else {
    ExternalEvent = KinovaFSM::Unfolded;
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
      ALL_LOG(logWARNING) << "KinovaArm: Requested Mode invalid.";
    } 
  }
  clock_gettime(CLOCK_REALTIME, &ModeChangeTimerStart);
}


/*Runs Timer and sends ModeChange Event when done.*/
void KinovaArm::modeChangeTimer() {
  timespec TimerNow;
  clock_gettime(CLOCK_REALTIME, &TimerNow);
  double elapsedTime = (TimerNow.tv_sec-ModeChangeTimerStart.tv_sec) * 1000 +
                    (TimerNow.tv_nsec-ModeChangeTimerStart.tv_nsec) / 1000000;
  if (elapsedTime > ModeChangeTimer) {
    ExternalEvent = KinovaFSM::ModeSet;
    ModeChangeTimer=0;
  }
}


/*Moves arm according to Joystick signal.*/
void KinovaArm::move() {
  double currentSpeedX = -JoystickX*JoystickCalcFactor;
  double currentSpeedY = -JoystickY*JoystickCalcFactor;
  double currentSpeedZ = JoystickZ*JoystickCalcFactor;
  currentPosition = -1;
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
      ALL_LOG(logDEBUG4) << "MoveArm In Mode: 1";
    }
    if (Mode == KinovaStatus::Rotation || Mode == KinovaStatus::Axis2) {
      axes.wrist_lr = currentSpeedX;
      axes.wrist_fb = currentSpeedY;
      axes.wrist_rot = currentSpeedZ;
      ALL_LOG(logDEBUG4) << "MoveArm In Mode: 2";
    }
    try {
      arm->move_joystick_axis(axes);
      ALL_LOG(logDEBUG4) << "currentSpeed: "
                         << "x= " << currentSpeedX
                         << ", y= " << currentSpeedY
                         << ", z= " << currentSpeedZ ;
    }
    catch( KinDrv::KinDrvException &e ) {
      error("move", e, false);
    }
    checkCurrents();
  }
}


/*Sets the Targetpoint for the Movement*/
void KinovaArm::setTarget(KinovaPts::Objective targetObjective) {
  if (targetObjective > 0 && targetObjective <= KinovaPts::NumberOfObjectives) {
    TargetObjective = targetObjective;
    PositionHandler.resetSequence();
    float currentCoordinates[6];
    getPosition(currentCoordinates);
    PositionHandler.setZeroObjective(targetObjective, currentCoordinates);
  }
  else {
    ALL_LOG(logWARNING) << "KinovaArm: invalid targetPosition.";
    ExternalEvent = KinovaFSM::SequenceDone;
  }
}


/*Moves the Arm towards the next Point in the sequenceList.
If Point is reached, sequence is counted up.
If end of sequence is reached, PositionReached event is sent.*/
void KinovaArm::moveToPosition(bool init) {
  float targetCoordinates[6];
  float currentCoordinates[6];
  currentPosition = 0;
  getPosition(currentCoordinates);
  //Check if Sequence is still going
  if ( PositionHandler.getCoordinates(targetCoordinates, TargetObjective, currentCoordinates) ) {
    //Check if in range
    bool PointReached = checkIfReached(targetCoordinates, currentCoordinates);
    bool currentLimit = checkCurrents();
    if (PointReached || currentLimit) {
      ALL_LOG(logDEBUG) << "Sequence-Point " << PositionHandler.getSequence() << " reached.";
      try {
        arm->erase_trajectories();
      }
      catch( KinDrv::KinDrvException &e ) {
        error("moveToPosition", e, false);
      }
      //Next Point in Sequence.
      PositionHandler.incrementSequence();
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
    currentPosition = TargetObjective;
    if (init) { InternalEvent = KinovaFSM::InitHomeReached; }
    else {
      ALL_LOG(logDEBUG) << "Sequence " << currentPosition << " done." ;
      ExternalEvent = KinovaFSM::SequenceDone;
    }
  }
}

void KinovaArm::sequenceDone() {
  PositionHandler.resetSequence();
  TargetObjective = KinovaPts::NoObjective;
}

/*sets the TeachingTarget (Objective at whitch will be teached). Keeps old objective and Sequence when called with Zero.*/
void KinovaArm::teachPosition(KinovaPts::Objective targetObjective) {
  if (targetObjective != 0) {
    if (targetObjective > 0 && targetObjective <= KinovaPts::NumberOfObjectives) {
      ALL_LOG(logDEBUG) << "new teachTarget, sequence reset.";
      float currentCoordinates[6];
      getPosition(currentCoordinates);
      PositionHandler.resetSequence();
      PositionHandler.setZeroObjective(targetObjective, currentCoordinates);
      TeachTarget = targetObjective;
    }
    else {
      ALL_LOG(logWARNING) << "KinovaArm: invalid teach Objective.";
    }
  }
  ALL_LOG(logDEBUG) << "TeachMode: Teaching at Point " << TeachTarget << ":"
                   << PositionHandler.getSequence();
}


/*moves Arm to the current point in the sequence at the current TeachTarget*/
void KinovaArm::moveToPoint() {
  float targetCoordinates[6];
  float currentCoordinates[6];
  currentPosition=0;

  getPosition(currentCoordinates);
  
  //Check if Sequence is still going
  if ( PositionHandler.getCoordinates(targetCoordinates, TeachTarget, currentCoordinates) ) {
    //Check if in range
    bool PointReached = checkIfReached(targetCoordinates, currentCoordinates);
    checkCurrents();
    if (PointReached) {
      currentPosition = getCurrentPoint();
      ExternalEvent = KinovaFSM::PointReached;
      ALL_LOG(logDEBUG) << "Point " << currentPosition
                        << " of Target: " << TeachTarget << " is reached.";
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
    ALL_LOG(logWARNING) << "moveToPoint: No such Point in Sequence.";
    ExternalEvent = KinovaFSM::PointNotReached;
  }
}


/*moves Arm to origin of current TeachTarget*/
void KinovaArm::moveToOrigin() {
  float targetCoordinates[6];
  float currentCoordinates[6];
  currentPosition=0;

  getPosition(currentCoordinates);

  //Check if Origin is defined
  if ( PositionHandler.getOrigin(targetCoordinates, TeachTarget, currentCoordinates) ) {
    //Check if in range
    bool PointReached = checkIfReached(targetCoordinates, currentCoordinates);
    checkCurrents();
    if (PointReached) {
      currentPosition = -1;
      ExternalEvent = KinovaFSM::PointReached;
      ALL_LOG(logDEBUG) << "Origin of " << TeachTarget << " reached.";
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
    ALL_LOG(logWARNING) << "moveToOrigin: No Origin defined.";
    ExternalEvent = KinovaFSM::PointNotReached;
  }
}


/*Saves current Arm-Position to current SequencePoint of teachTarget*/
void KinovaArm::savePoint(int EventVariable) {
  float currentCoordinates[6];

  if (EventVariable == PositionHandler.getSequence()) {
    getPosition(currentCoordinates);

    PositionHandler.savePoint(currentCoordinates, TeachTarget);
    PositionHandler.writeToFile();
    ExternalEvent = KinovaFSM::PointSaved;
  }
  ExternalEvent = KinovaFSM::PointSaved;
}


/*save current Arm-Position as new origin of Coordinate System*/
void KinovaArm::saveOrigin() {
  float currentCoordinates[6];

  getPosition(currentCoordinates);

  PositionHandler.saveOrigin(currentCoordinates, TeachTarget);
  PositionHandler.writeToFile();
  ExternalEvent = KinovaFSM::OriginSaved;

}

void KinovaArm::deletePoint() {
  PositionHandler.deletePoint(TeachTarget);
  PositionHandler.writeToFile();  
  ExternalEvent = KinovaFSM::PointDeleted;
}


/*decrements Sequence if EventVariable is the expected point after decrement.*/
void KinovaArm::previousPoint(int EventVariable) {
  int currentSequence = PositionHandler.getSequence();
  if (EventVariable == currentSequence-1) {
    PositionHandler.decrementSequence();
    ExternalEvent = KinovaFSM::PreviousPointSet;
  }
  else if(EventVariable == currentSequence) {
    ExternalEvent = KinovaFSM::PreviousPointSet;
  }
  else {
    ExternalEvent = KinovaFSM::PreviousPointNotSet;
  }
}


/*increments Sequence if EventVariable is the expected point after increment.*/
void KinovaArm::nextPoint(int EventVariable) {
  int currentSequence = PositionHandler.getSequence();
  if (EventVariable == currentSequence+1) {
    PositionHandler.incrementSequence();
    ExternalEvent = KinovaFSM::NextPointSet;
  }
  else if(EventVariable == currentSequence) {
    ExternalEvent = KinovaFSM::NextPointSet;
  }
  else {
    ExternalEvent = KinovaFSM::NextPointNotSet;
  }
}


/*PrintCurrents if greater than 2. returns True if Currents greater than 3.5, sends Error if Current is greater than 4.5 (fuse at 5 A)*/
bool KinovaArm::checkCurrents() {
  try {
    KinDrv::jaco_position_t current = arm->get_ang_current();
    float sumCurrent = 0;
    for (int i = 0; i < 6; i++) {
      sumCurrent += current.joints[i];
    }
    if (sumCurrent > 2) {
      ALL_LOG(logDEBUG2) << "getCurrents: " << "(" << current.joints[0] << ", "
                                                   << current.joints[1] << ", "
                                                   << current.joints[2] << ", "
                                                   << current.joints[3] << ", "
                                                   << current.joints[4] << ", "
                                                   << current.joints[5] << ")";
    }
    if (sumCurrent > 4.5) {
      error("checkCurrents", "Overcurrent");
      Connected = false;
      Error = true;
    }
    if (sumCurrent > 3.5) {
      return true;
    }
    return false;
  }
  catch( KinDrv::KinDrvException &e ) {
    error("checkCurrents", e, false);
  }
}


/*Set-Functinons*/
void KinovaArm::setJoystick(int x, int y, int z) {
  ALL_LOG(logDEBUG4) << "KinovaArm::setJoystick (" << x << ", "
                                                   << y << ", "
                                                   << z << ")";
  JoystickX = x;
  JoystickY = y;
  JoystickZ = z;
}

/*Get-Functions*/
bool KinovaArm::getError() { return Error; }
bool KinovaArm::getInitialize() { return Initialized; }
bool KinovaArm::getActive() { return Active; }
int  KinovaArm::getMode() { return Mode; }
int  KinovaArm::getCurrentPosition() { return currentPosition; }
int  KinovaArm::getCurrentPoint() { return PositionHandler.getSequence(); }

KinovaFSM::Event KinovaArm::getExternalEvent() {
  ALL_LOG(logDEBUG4) << "KinovaArm::getExternalEvent: " << KinovaFSM::EventName[ExternalEvent];
  KinovaFSM::Event e = ExternalEvent;
  ExternalEvent = KinovaFSM::NoEvent;
  return e;
}

KinovaFSM::Event KinovaArm::getInternalEvent() {
  ALL_LOG(logDEBUG4) << "KinovaArm::getInternalEvent: " << KinovaFSM::EventName[ExternalEvent];
  KinovaFSM::Event e = InternalEvent;
  InternalEvent = KinovaFSM::NoEvent;
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
  ALL_LOG(logDEBUG4) << "KinovaArm::getPosition: currentCoordinates: ("
                     << coordinates[0] << ", "
                     << coordinates[1] << ", "
                     << coordinates[2] << ", "
                     << coordinates[3] << ", "
                     << coordinates[4] << ", "
                     << coordinates[5] << ")" ;
}

void  KinovaArm::setActive() { Active = true; }
void  KinovaArm::setInactive() {
  Active = false;
  Mode = KinovaStatus::NoMode;
}


/*Returns true if arm isn't moving anymore. Starts after initial timer of 200ms to wait for arm to start moving.*/
bool KinovaArm::checkIfReached(float* targetCoordinates, float* currentCoordinates) {
  if (MoveTimerStart.tv_sec == 0 && MoveTimerStart.tv_nsec == 0 ) {
    clock_gettime(CLOCK_REALTIME, &MoveTimerStart);
  }
  timespec TimerNow;
  clock_gettime(CLOCK_REALTIME, &TimerNow);
  double elapsedTime = (TimerNow.tv_sec-MoveTimerStart.tv_sec) * 1000 +
                       (TimerNow.tv_nsec-MoveTimerStart.tv_nsec) / 1000000;

  if (elapsedTime > 500) {
    bool pointReached = true;
    ALL_LOG(logDEBUG4) << "KinovaArm::RangeCheck: following Axis do not pass:";
    for (int i = 0; i<6; i++) {
      float dVel = fabs( currentCoordinates[i] - LastCoordinates[i] );
      if ( dVel > VELOCITY_RANGE ) {
        ALL_LOG(logDEBUG4) << "[" << i << "]: dVel: " << dVel;
        pointReached = false; 
      }
      LastCoordinates[i] = currentCoordinates[i];
    }
    //Returns PointReached if PointReachedCount is higher than 3, to prevent errors.
    if (pointReached) {
      ++PointReachedCount;
    }
    else {
      PointReachedCount = 0;
    }
    if (PointReachedCount>3) {
      PointReachedCount = 0;
      MoveTimerStart.tv_sec = 0;
      MoveTimerStart.tv_nsec = 0;
      return pointReached;
    }
  }
  return false;
}


/*PrintForces. TODO: return Forces */
void KinovaArm::getForces() {
  try {
    KinDrv::jaco_position_t force = arm->get_ang_force();
    ALL_LOG(logDEBUG2) << "getForces:" << "(" << force.joints[0] << ", "
                                              << force.joints[1] << ", "
                                              << force.joints[2] << ", "
                                              << force.joints[3] << ", "
                                              << force.joints[4] << ", "
                                              << force.joints[5] << ")";
  }
  catch( KinDrv::KinDrvException &e ) {
    error("getForces", e, false);
  }
}
