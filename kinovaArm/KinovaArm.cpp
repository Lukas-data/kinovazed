#include <stdio.h>
#include "KinovaArm.h"
#include "PositionHandling.h"


  
#define KINOVA_CONNECTED true


using namespace KinDrv;


KinovaArm::KinovaArm()
{
  if (KINOVA_CONNECTED == true) {

    // explicitly initialize a libusb context; optional
    KinDrv::init_usb();
  

    printf("Try to Connect to JacoArm \n");
    try {
      arm = new JacoArm();
      printf("Successfully connected to JacoArm! \n");
    } catch( KinDrv::KinDrvException &e ) {
      printf("error %i: %s \n", e.error(), e.what());
      throw std::runtime_error("Connection to JacoArm failed\n");
    }
  }
  calcFactor = 0.0025;
  EmergencyStop = false;

}


KinovaArm::~KinovaArm()
{
  if (KINOVA_CONNECTED == true) {
    // explicitly close libusb context (only needed if explicitly openede before)
    close_usb();
    delete arm;
    printf("Connection to JacoArm closed \n");
  }
}


void KinovaArm::JacoMain()
{
  if (EmergencyStop == false) {
    if (DoInitialize == true || Initializing == true)
      initialize();
    if (Ready == true && MovingToPosition == false) //requestedRobotMode != currentRobotMode entfernt -> ToBeTested
      setMode();
    //printf("currentRobotMode: %d, Ready = %d, SettingMode = %d\n",currentRobotMode, Ready, SettingMode);
    if (currentRobotMode > 0 && currentRobotMode < 5 && Ready == true && SettingMode == false && MovingToPosition == false)
      moveArm();
    //printf("MovingToPosition: %d, Ready = %d\n", MovingToPosition, Ready);
    if ( MovingToPosition == true  && Ready == true)
      moveToPos();
  }
  else if (Ready == true) {
    printf("E-Stop\n");
    stopArm();
  }

  printf("requestedPosition: %d // ", requestedPosition);
  printf("currentPosition: %d\n", currentPosition);
  ////TEST
  /*
  PositionHandling::printPos();
  Position = arm->get_cart_pos();
  printf("Pos: ");
  for (int i = 0; i <3; ++i)
    printf("%f, ", Position.position[i]);
  printf("Rot: ");
  for (int i = 0; i <2; ++i)
    printf("%f ,", Position.rotation[i]);
  printf("%f \n", Position.rotation[2]);
  

  float* test = PositionHandling::getCoordinates(0);
  printf("Home.x: %f\n", test[2]);
  */
}


void KinovaArm::DoEmergencyStop() {
  if (Ready == true) {
    stopArm();
    arm->stop_api_ctrl();
  }
  EmergencyStop = true;
  Ready = false;
  currentRobotMode = 0;
  requestedRobotMode = 0;
  printf("E-Stop set\n");
  
  
}

void KinovaArm::DoQuitEmergencyStop() {
  EmergencyStop = false;
  writeDataToBuffer(ESTOP_QUIT);
  printf("E-Stop quitted\n");
}

void KinovaArm::setDoInitialize() {
  if (EmergencyStop == false) {
    DoInitialize = true;
  //printf("start Initializing...\n");
  }
}

//Initialisation of Jaco Arm
void KinovaArm::initialize()
{
  //check if allready initialized
  if (Ready == true) {
    DoInitialize = false;
    writeDataToBuffer(INIT);
    printf("Arm is allready initialized\n");
  }
  // Establish Connection with Arm
  else if (KINOVA_CONNECTED == true) {
    if (Initializing == false) {
      printf("Gaining API control over the arm \n");
      arm->start_api_ctrl();

      //print state of Arm
      Status = arm->get_status();
      printf("Arm is currently in state: %i \n", Status);

      currentPosition = POS_NOPOS;
      requestedPosition = POS_NOPOS;

      Initializing = true;
      DoInitialize = false;
    }

    Status = arm->get_status();
    //Moves Arm to Kinova Home position if not in initialized Position
    if( Status == MODE_NOINIT) {
      printf("Initializing: Status is NOINIT.\n");
      //push the "HOME/RETRACT" button until arm is initialized
      jaco_joystick_button_t buttons = {0};
      buttons[2] = 1;
      arm->push_joystick_button(buttons);
      MovingHome = true;
    }
    //Moves Arm to Own Home position after it moved to initializable Position
    else if (MovingHome == true) {
      if ( requestedPosition == POS_NOPOS ) {
        arm->release_joystick();
        //Initialization moves to Position 0 (ZEDe-Home)
        DoMoveToPos(POS_ZEDHOME);
        printf("Arm is moving to ZEDe-Home Position, state: %i \n", Status);
      }
      else if ( currentPosition != POS_ZEDHOME) {
        moveToPos();
      }
      else {
        Initializing = false;
        Ready = true;
        MovingHome == false;
        writeDataToBuffer(INIT);
        printf("Arm initialized at ZED-Home, state: %i \n", Status);
      }
    }
    else if (MovingHome == false){
      arm->release_joystick();
      //Initialization moves to Own-Homeposition (for Testing Position)
      //DoMoveToPos(0);
      //MovingToPosition = false;
      Initializing = false;
      Ready = true;
      writeDataToBuffer(INIT);
      //DoSetMode(1);
      printf("Arm initialized without moving, state: %i \n", Status);

    }
  }
  else if (KINOVA_CONNECTED == false) {
    Initializing = false;
    Ready = true;
    DoInitialize = false;
    writeDataToBuffer(INIT);
    printf("Arm connection is turned off. Dummy-Initialized.\n");
  }
}


void KinovaArm::DoSetMode(int mode) {
  if (EmergencyStop == false) {
    if (mode>0 && mode < 5) {
      if (mode == currentRobotMode) {
        printf("Mode allready set to %d\n",currentRobotMode);
        writeDataToBuffer(MODE_SET);
      }
      else if (mode != requestedRobotMode) {
        //SetRequestedKinematicsMode
        if (mode == 1 || mode == 2)
          requestedKinematicsMode = 1;
        if (mode == 3 || mode == 4)
          requestedKinematicsMode = 2;
        //SetRequestedJSMode
        if (mode == 1 || mode == 3)
          requestedJSMode = 1;
        if (mode == 2 || mode == 4)
          requestedJSMode = 2;

        SettingMode = true;  
        setModeCycleCount = 0;
        requestedRobotMode = mode;   
      }
    }
    else
      requestedRobotMode = currentRobotMode;  //Requested Mode unknown
  //printf("setting Mode...\n"); 
  }
}


void KinovaArm::setMode() {
  //Check if something needs to be done.
  if (SettingMode == true) {
    if(requestedKinematicsMode == currentKinematicsMode) {
      //Count Down CycleCount, if KinematicsMode didn't change: Timer is allready 0.
      if (setModeCycleCount == 0) {
        currentJSMode = requestedJSMode;
        currentRobotMode = requestedRobotMode;
        printf("Mode set to %d\n", requestedRobotMode);
        writeDataToBuffer(MODE_SET);
        SettingMode = false;
      }
      else
        --setModeCycleCount;
    }
    //Change KinematicsMode and start Timer to wait 50 Cycles.
    else {
      //Start Counter
      setModeCycleCount = 50;
      //Set Arm-Control-Mode
      if (KINOVA_CONNECTED == true) {
        if (requestedKinematicsMode == 1)
          arm->set_control_cart();
        if (requestedKinematicsMode == 2)
          arm->set_control_ang();
      }
      //Mode is set. Returns as soon as CycleCount is 0.
      currentKinematicsMode = requestedKinematicsMode;      
    }  
  }
}


void KinovaArm::DoSetSpeed(int x, int y, int z)
{
  if (EmergencyStop == false) {
    requestedPosition = POS_NOPOS;
    currentPosition = POS_NOPOS;
    currentSpeedX = -x*calcFactor;
    currentSpeedY = -y*calcFactor;
    currentSpeedZ = z*calcFactor;
    //printf("SpeedSet in Mode %d set to: x= %f, y= %f, z= %f\n",currentRobotMode, currentSpeedX,currentSpeedY,currentSpeedZ);
  }
  else {
    currentSpeedX = 0;
    currentSpeedY = 0;
    currentSpeedZ = 0;
  }
}

void KinovaArm::moveArm() {
  

  if (KINOVA_CONNECTED == true) {
  axes.trans_lr = 0; 
  axes.trans_fb = 0;
  axes.trans_rot = 0;
  axes.wrist_lr = 0;
  axes.wrist_fb = 0;
  axes.wrist_rot = 0;

    if (currentJSMode == 1 && EmergencyStop == false) {
      axes.trans_lr = currentSpeedX; 
      axes.trans_fb = currentSpeedY;
      axes.trans_rot = currentSpeedZ;
      axes.wrist_lr = 0;
      axes.wrist_fb = 0;
      axes.wrist_rot = 0;
    }

    if (currentJSMode == 2 && EmergencyStop == false) {
      axes.trans_lr = 0; 
      axes.trans_fb = 0;
      axes.trans_rot = 0; 
      axes.wrist_lr = currentSpeedX;
      axes.wrist_fb = currentSpeedY;
      axes.wrist_rot = currentSpeedZ;  

    }
  arm->move_joystick_axis(axes);
  }
  //printf("MoveArm In Mode: %d with x= %f, y= %f, z= %f\n",currentRobotMode, currentSpeedX,currentSpeedY,currentSpeedZ);
}


void KinovaArm::stopArm() {
  if (KINOVA_CONNECTED == true) {
    axes.trans_lr = 0; 
    axes.trans_fb = 0;
    axes.trans_rot = 0;
    axes.wrist_lr = 0;
    axes.wrist_fb = 0;
    axes.wrist_rot = 0;
    arm->move_joystick_axis(axes);
    arm->release_joystick();
  }
}


void KinovaArm::DoMoveToPos(int pos) {
  printf("Moving to Position: %d\n", pos);
  float fingers[3];
  requestedPosition = pos;
  PositionHandling::getCoordinates(requestedPositionCord, pos);
  MovingToPosition = true;
  arm->set_target_cart(requestedPositionCord,fingers);
}


void KinovaArm::moveToPos() {
  bool PositionReached = true;

  jaco_position_t Position = arm->get_cart_pos();
  
  for (int i = 0; i<6; i++) {
    if (i<3) 
      currentPositionCord[i] == Position.position[i];
    else
      currentPositionCord[i] == Position.rotation[i-3];
  }
  //printf("currentPositionCord Reset.\n");

  printf("Following Directions are within Range: ");
  for (int i = 0; i<6; i++) {
    if (currentPositionCord[i] > requestedPositionCord[i] * (1+POSITION_RANGE) &&
        currentPositionCord[i] < requestedPositionCord[i] * (1-POSITION_RANGE) ) {
      printf("%d, ", i);
      PositionReached = false;
    }
    printf("\n");
  }
  if (PositionReached == true) {
    MovingToPosition = false;
    arm->release_joystick();
    currentPosition = requestedPosition;
    DoSetMode(1);
  }
}


int KinovaArm::getCurrentSpeedX() {return currentSpeedX;}
int KinovaArm::getCurrentSpeedY() {return currentSpeedY;}
int KinovaArm::getCurrentSpeedZ() {return currentSpeedZ;}

int KinovaArm::getDataToSend() {
  if (EmergencyStop == false)
    return getDataFromBuffer();
  else
    return ESTOP;
}

int KinovaArm::getCurrentMode() {return currentRobotMode;}

void KinovaArm::writeDataToBuffer(int Data) {
  for(int i = 0; i<8; i++) {
    if (DataBuffer[i] == 0) {
      DataBuffer[i] = Data;
      break;
    }
    if (i==7) {
      printf("KinovaArm: BufferOverflow!");
    }
  }
}

int KinovaArm::getDataFromBuffer() {
  int retData = DataBuffer[0];
  for(int i= 0; i<7; i++) {
    DataBuffer[i] = DataBuffer[i+1];
  }
  DataBuffer[7] = 0;
  return retData;
}

void KinovaArm::clearBuffer() {
  for(int i= 0; i<8; i++) {
    DataBuffer[i] = 0;
  }
}


void KinovaArm::getArmStatus() {
//  jaco_retract_mode_t ArmStatus;
 // try {
 //   ArmStatus = arm->get_status();
//  }
 // catch ( KinDrv::KinDrvException &e ){
 //   printf("Connection to Jaco-Arm lost!\n");
    
  //}
 // printf("ArmStatus: %d\n", ArmStatus);
/*
  jaco_position_t CartPos = arm->get_cart_pos();
  printf("CartPos.joints: ");
  for (int i = 0; i<5; ++i) {
    printf(" %f,", CartPos.joints[i]);
  }
  printf(" %f\n", CartPos.joints[5]);
 */ 
}


/*
int KinovaArm::goto_retract()
{
  // this can only be achieved from HOME position. Otherwise the arm
  // will move to HOME. You'll probably need to uncomment the gripper movements
  // in order for this to work. Or even better, implement moving to HOME position,
  // which could be triggered before going to RETRACT ;)
  jaco_retract_mode_t mode = arm->get_status();
  switch( mode ) {
    case MODE_READY_TO_RETRACT:
      // is currently on the way to RETRACT. Need 2 button presses,
      // 1st moves towards HOME, 2nd brings it back to its way to RETRACT
      arm->push_joystick_button(2);
      arm->release_joystick();
      arm->push_joystick_button(2);
      break;

    case MODE_READY_STANDBY:
    case MODE_RETRACT_TO_READY:
      // just 1 button press needed
      arm->push_joystick_button(2);
      break;

    case MODE_NORMAL_TO_READY:
    case MODE_NORMAL:
    case MODE_NOINIT:
      printf("cannot go from NORMAL/NOINIT to RETRACT \n");
      return 0;
      break;

    case MODE_ERROR:
      printf("some error?! \n");
      return 0;
      break;

    case MODE_RETRACT_STANDBY:
      printf("nothing to do here \n");
      return 1;
      break;
  }

  while( mode != MODE_RETRACT_STANDBY ) {
    usleep(1000*10); // 10 ms
    mode = arm->get_status();
  }
  arm->release_joystick();

  return 1;
}
*/
int KinovaArm::goto_home()
{
  Status = arm->get_status();
  printf("goToHome - Status : %d, MovingPosition: %d\n", Status, MovingToPosition);
  // going to HOME position is possible from all positions. Only problem is,
  // if there is some kind of error
  if (MovingToPosition == false) {
    jaco_joystick_button_t buttons = {0};
    buttons[2] = 1;
    switch( Status ) {
      case MODE_RETRACT_TO_READY:
        MovingToPosition = true;
        // is currently on the way to HOME. Need 2 button presses,
        // 1st moves towards RETRACT, 2nd brings it back to its way to HOME
        arm->push_joystick_button(buttons);
        arm->release_joystick();
        arm->push_joystick_button(buttons);
        //printf("Button Pushed_twice\n");
        break;

      case MODE_NORMAL_TO_READY:
      case MODE_READY_TO_RETRACT:
      case MODE_RETRACT_STANDBY:
      case MODE_NORMAL:
      case MODE_NOINIT:
        MovingToPosition = true;
        // just 1 button press needed
        arm->push_joystick_button(buttons);
        //printf("Button Pushed_once\n");
        break;

      case KinDrv::MODE_ERROR:
        printf("some error?! \n");
        break;

      case KinDrv::MODE_READY_STANDBY:
        printf("nothing to do here \n");
        break;
    }
  }
  else {
    if( Status != MODE_READY_STANDBY ) {
      //printf("Button is pushing!\n");
      if( Status == KinDrv::MODE_READY_TO_RETRACT ) {
        printf("GoToHome: Status is Ready to Retract.\n");
        arm->release_joystick();
        arm->push_joystick_button(2);
      }
    }
    else {
      printf("Arm is in Home Position.\n");
      arm->release_joystick();
      MovingToPosition == false;
    }
  }
}







