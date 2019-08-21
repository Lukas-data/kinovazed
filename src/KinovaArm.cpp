#include "KinovaArm.h"
#include "Coordinates.h"
#include "Log.h"

#include <array>
#include <chrono>

constexpr auto kinovaDummy = false;

KinovaArm::~KinovaArm() {
	disconnect();
}

/*Prints Error/Warning-Message.Sets Errorflag and removes Connected flag on Error.*/
void KinovaArm::error(const char *funcName, KinDrv::KinDrvException const &e, bool warning) {
	if (warning) {
		ALL_LOG(logWARNING) << "KinovaArm::" << funcName << "(): " << e.error() << ", " << e.what();
	} else {
		ALL_LOG(logERROR) << "KinovaArm::" << funcName << "(): " << e.error() << ", " << e.what();
		connected = false; //Simplification!!
		Error = true;
	}
}
void KinovaArm::error(const char *funcName, const char *errorMsg) {
	ALL_LOG(logERROR) << "KinovaArm::" << funcName << "(): " << errorMsg;
	connected = false; //Simplification!!
	Error = true;
}

/*Tries to connect to KinovaArm*/
bool KinovaArm::connect() {
	if (kinovaDummy || connected) {
		connected = true;
		return true;
	} else {
		try {
			//KinovaArm::disconnect();
			arm = new KinDrv::JacoArm();
			connected = true;
			ALL_LOG(logINFO) << "Connection to JacoArm established.";
			return true;
		} catch (KinDrv::KinDrvException const& e) {
			error("connect", e, false);
			return false;
		}
	}
}

/*Disconnects to KinovaArm*/
void KinovaArm::disconnect() {
	if (!kinovaDummy) {
		try {
			if (arm != nullptr) {
				delete arm;
			}
			ALL_LOG(logINFO) << "Connection to JacoArm closed. ";
		} catch (KinDrv::KinDrvException const &e) {
			error("disconnect", e, false);
		}
	}
	connected = false;
}

void KinovaArm::reconnectOnError() {
	if (Error) {
		ALL_LOG(logDEBUG3) << "Connection to JacoArm reopend after Error.";
		if (connected) {
			disconnect();
		}
		Error = !connect();
	}
}

/*Gain API Control*/
void KinovaArm::takeControl() {
	if (kinovaDummy) {
		return;
	}
	try {
		arm->start_api_ctrl();
		ALL_LOG(logINFO) << "Gained API control over the arm.";
	} catch (KinDrv::KinDrvException const &e) {
		error("takeControl", e, false);
	}
}

/*Release API Control*/
void KinovaArm::releaseControl() {
	if (!kinovaDummy) {
		try {
			arm->stop_api_ctrl();
			ALL_LOG(logINFO) << "Released API control over the arm.";
			Error = false;
		} catch (KinDrv::KinDrvException const &e) {
			error("releaseControl", e, false);
		}
	}
	initialized = false;
}

/*Stops arm and erases all trajectories.*/
void KinovaArm::dontMove() {
	if (kinovaDummy) {
		return;
	}
	//reset Movement-Target
	try {
		arm->erase_trajectories();
	} catch (KinDrv::KinDrvException const &e) {
		error("dontMove: stopping Movement", e, false);
	}
	//reset Joystick-input
	KinDrv::jaco_joystick_axis_t axes;
	axes.trans_lr = 0;
	axes.trans_fb = 0;
	axes.trans_rot = 0;
	axes.wrist_lr = 0;
	axes.wrist_fb = 0;
	axes.wrist_rot = 0;
	try {
		arm->move_joystick_axis(axes);
		arm->release_joystick();
	} catch (KinDrv::KinDrvException const &e) {
		error("dontMove: releasing Joystick", e, false);
	}
}

/*Checks if Arm is allready initialized and sets Home as targetPosition if not.*/
void KinovaArm::checkInitialize() {
	initialized = false;
	PositionHandler.init();
	if (kinovaDummy) {
		initialized = true;
		externalEvent = KinovaFSM::Initialized;
		ALL_LOG(logINFO) << "Arm connection is turned off. Dummy-Initialized.";
		return;
	}
	try {
		startRetracting(true);
	} catch (KinDrv::KinDrvException const &e) {
		error("initialize", e, false);
	}
}

/*Initialize Arm and go to Home position*/
void KinovaArm::initialize() {
	currentPosition = 0;
	if (kinovaDummy) {
		initialized = true;
		externalEvent = KinovaFSM::Initialized;
		ALL_LOG(logINFO) << "Arm connection is turned off. Dummy-Initialized.";
		return;
	}
	if (!initialized) {
		try {
			retract(true);
		} catch (KinDrv::KinDrvException const &e) {
			error("initialize", e, false);
		}
	}
}

/*Pushes Button if Arm is not allready retracted*/
void KinovaArm::startRetracting(bool init) {
	currentPosition = 0;
	if (kinovaDummy) {
		return;
	}
	homed = false;
	try {
		KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
		if (armStatus != 3) {
			ALL_LOG(logDEBUG) << "Arm starts retracting from Mode: " << armStatus;
			arm->push_joystick_button(2);
			ALL_LOG(logDEBUG2) << "Start Retracting: Button pressed!";
			armStatus = arm->get_status();
		} else {
			arm->release_joystick();
			ALL_LOG(logDEBUG2) << "Already retracted: Button released!";
			if (!init) {
				externalEvent = KinovaFSM::Retracted;
			} else {
				initialized = true;
				externalEvent = KinovaFSM::Initialized;
			}

		}
	} catch (KinDrv::KinDrvException const &e) {
		error("retract", e, false);
	}
}

/*brings arm to retracted position*/
void KinovaArm::retract(bool init) {
	ALL_LOG(logDEBUG3) << "Arm tries to retract!";
	currentPosition = 0;
	if (kinovaDummy) {
		return;
	}
	try {
		KinDrv::jaco_retract_mode_t armStatus = arm->get_status();
		if (!homed && armStatus == KinDrv::MODE_READY_STANDBY) {
			homed = true;
			arm->release_joystick();
			ALL_LOG(logDEBUG2) << "Retract from Home: Button released!";
			armStatus = arm->get_status();
			arm->push_joystick_button(2);
			ALL_LOG(logDEBUG2) << "Retract from Home: Button pressed!";
			armStatus = arm->get_status();
		} else if (homed && armStatus == KinDrv::MODE_READY_TO_RETRACT) {
			homed = false;
		} else if (armStatus == KinDrv::MODE_RETRACT_TO_READY) {
			arm->release_joystick();
			ALL_LOG(logDEBUG2) << "Retract to Home: Button released!";
			arm->push_joystick_button(2);
			ALL_LOG(logDEBUG2) << "Retract to Home: Button pressed!";
		} else if (armStatus == KinDrv::MODE_RETRACT_STANDBY) {
			arm->release_joystick();
			ALL_LOG(logDEBUG2) << "Retracted: Button released!";
			if (!init) {
				externalEvent = KinovaFSM::Retracted;
			} else {
				initialized = true;
				externalEvent = KinovaFSM::Initialized;
			}
		} else if (armStatus == KinDrv::MODE_ERROR) {
			error("retract", "Arm has an Error!");
		}
	} catch (KinDrv::KinDrvException const &e) {
		error("retract", e, false);
	}
}

/*Moves Arm out of retracted Position to own home position.*/
void KinovaArm::unfold() {
	if (kinovaDummy) {
		externalEvent = KinovaFSM::Unfolded;
		return;
	}
	if (currentPosition != Kinova::Home) {
		if (TargetObjective != Kinova::Home) {
			setTarget(Kinova::Home);
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
				//tcorbat: Discuss whether break is required here
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
		} catch (KinDrv::KinDrvException const &e) {
			error("unfold", e, false);
		}
	} else {
		externalEvent = KinovaFSM::Unfolded;
	}
}

/*Changes current Mode and sets TimerTime (only necessary if kinematic 
 mode on Jaco needs to be changed.). Default nextMode = NoMode is Translation!*/
void KinovaArm::changeMode(KinovaStatus::SteeringMode nextMode) {
	if (mode != nextMode || mode == KinovaStatus::NoMode) {
		if (!kinovaDummy) {
			try {
				if ((mode != KinovaStatus::Axis1 && mode != KinovaStatus::Axis2)
						&& (nextMode == KinovaStatus::Axis1 || nextMode == KinovaStatus::Axis2)) {
					arm->set_control_ang();
					maxModeChangeTimer = std::chrono::milliseconds { 500 };
				}
				if ((mode != KinovaStatus::Translation && mode != KinovaStatus::Rotation)
						&& (nextMode == KinovaStatus::Translation || nextMode == KinovaStatus::Rotation || nextMode == KinovaStatus::NoMode)) {
					arm->set_control_cart();
					maxModeChangeTimer = std::chrono::milliseconds { 500 };
				}
			} catch (KinDrv::KinDrvException const &e) {
				error("changeMode", e, false);
			}
		}
		switch (nextMode) {
		case KinovaStatus::NoMode:
			mode = KinovaStatus::Translation;
			break;
		case KinovaStatus::Translation:
		case KinovaStatus::Rotation:
		case KinovaStatus::Axis1:
		case KinovaStatus::Axis2:
			mode = nextMode;
			break;
		default:
			ALL_LOG(logWARNING) << "KinovaArm: Requested Mode invalid.";
		}
	}
	modeChangeTimerStart = std::chrono::steady_clock::now();
}

/*Runs Timer and sends ModeChange Event when done.*/
void KinovaArm::modeChangeTimer() {
	if (!modeChangeTimerStart) {
		modeChangeTimerStart = std::chrono::steady_clock::now();
	}
	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - *modeChangeTimerStart) > maxModeChangeTimer) {
		externalEvent = KinovaFSM::ModeSet;
		maxModeChangeTimer = std::chrono::milliseconds { 0 };
	}
}

/*Moves arm according to Joystick signal.*/
void KinovaArm::move() {
	double currentSpeedX = -joystickX * joystickCalcFactor;
	double currentSpeedY = -joystickY * joystickCalcFactor;
	double currentSpeedZ = joystickZ * joystickCalcFactor;
	currentPosition = -1;
	if (kinovaDummy) {
		return;
	}
	KinDrv::jaco_joystick_axis_t axes;
	axes.trans_lr = 0;
	axes.trans_fb = 0;
	axes.trans_rot = 0;
	axes.wrist_lr = 0;
	axes.wrist_fb = 0;
	axes.wrist_rot = 0;

	if (mode == KinovaStatus::Translation || mode == KinovaStatus::Axis1) {
		axes.trans_lr = currentSpeedX;
		axes.trans_fb = currentSpeedY;
		axes.trans_rot = currentSpeedZ;
		ALL_LOG(logDEBUG4) << "MoveArm In Mode: 1";
	}
	if (mode == KinovaStatus::Rotation || mode == KinovaStatus::Axis2) {
		axes.wrist_lr = currentSpeedX;
		axes.wrist_fb = currentSpeedY;
		axes.wrist_rot = currentSpeedZ;
		ALL_LOG(logDEBUG4) << "MoveArm In Mode: 2";
	}
	try {
		arm->move_joystick_axis(axes);
		ALL_LOG(logDEBUG4) << "currentSpeed: " << "x= " << currentSpeedX << ", y= " << currentSpeedY << ", z= " << currentSpeedZ;
	} catch (KinDrv::KinDrvException const &e) {
		error("move", e, false);
	}
	checkCurrents();
}

/*Sets the Targetpoint for the Movement*/
void KinovaArm::setTarget(Kinova::Objective targetObjective) {
	if (targetObjective > 0 && targetObjective <= Kinova::NumberOfObjectives) {
		PositionHandler.resetSequence(TargetObjective);
		PositionHandler.resetSequence(targetObjective);
		TargetObjective = targetObjective;
		std::array<float, 6> currentCoordinates{};
		getPosition(currentCoordinates.data());
		PositionHandler.setZeroObjective(Kinova::Coordinates{currentCoordinates}, targetObjective);
	} else {
		ALL_LOG(logWARNING) << "KinovaArm: invalid targetPosition.";
		externalEvent = KinovaFSM::SequenceDone;
	}
}

/*Moves the Arm towards the next Point in the sequenceList.
 If Point is reached, sequence is counted up.
 If end of sequence is reached, PositionReached event is sent.*/
void KinovaArm::moveToPosition(bool init) {
	float currentCoordinates[6];
	currentPosition = 0;
	getPosition(currentCoordinates);
	//Check if Sequence is still going
	if (TargetObjective != Kinova::NoObjective && !PositionHandler.resetOriginAtEnd(TargetObjective)) {
		 auto targetCoordinates = PositionHandler.getCoordinates(TargetObjective);
		//Check if in range
		bool PointReached = checkIfReached(currentCoordinates);
		bool currentLimit = checkCurrents();
		if (PointReached || currentLimit) {
			ALL_LOG(logDEBUG) << "Sequence-Point " << PositionHandler.getSequence(TargetObjective) << " reached.";
			try {
				arm->erase_trajectories();
			} catch (KinDrv::KinDrvException &e) {
				error("moveToPosition", e, false);
			}
			//Next Point in Sequence.
			PositionHandler.incrementSequence(TargetObjective);
		} else {
			//Move to Position
			float fingers[3];
			try {
				std::array<float, 6> coordinatesData = targetCoordinates;
				arm->set_target_cart(coordinatesData.data(), fingers);
			} catch (KinDrv::KinDrvException const &e) {
				error("moveToPosition", e, false);
			}
		}
	} else {
		currentPosition = TargetObjective;
		if (init) {
			internalEvent = KinovaFSM::InitHomeReached;
		} else {
			ALL_LOG(logDEBUG) << "Sequence " << currentPosition << " done.";
			externalEvent = KinovaFSM::SequenceDone;
		}
	}
}

void KinovaArm::sequenceDone() {
	PositionHandler.resetSequence(TargetObjective);
	TargetObjective = Kinova::NoObjective;
}

/*sets the TeachingTarget (Objective at which will be taught). Keeps old objective and Sequence when called with Zero.*/
void KinovaArm::teachPosition(Kinova::Objective targetObjective) {
	if (targetObjective != Kinova::NoObjective) {
		if (Kinova::isValidObjective(targetObjective)) {
			ALL_LOG(logDEBUG) << "new teachTarget, sequence reset.";
			std::array<float, 6> currentCoordinates{};
			getPosition(currentCoordinates.data());
			PositionHandler.resetSequence(teachTarget);
			PositionHandler.resetSequence(targetObjective);
			PositionHandler.setZeroObjective(Kinova::Coordinates{currentCoordinates}, targetObjective);
			teachTarget = targetObjective;
		} else {
			ALL_LOG(logWARNING) << "KinovaArm: invalid teach Objective.";
		}
	}
	ALL_LOG(logDEBUG) << "TeachMode: Teaching at Point " << teachTarget << ":" << PositionHandler.getSequence(teachTarget);
}

/*moves Arm to the current point in the sequence at the current TeachTarget*/
void KinovaArm::moveToPoint() {
	float currentCoordinates[6];
	currentPosition = 0;

	getPosition(currentCoordinates);

	//Check if Sequence is still going
	if (teachTarget != Kinova::NoObjective && !PositionHandler.resetOriginAtEnd(teachTarget)) {
		auto targetCoordinates = PositionHandler.getCoordinates(teachTarget);
		//Check if in range
		bool PointReached = checkIfReached(currentCoordinates);
		checkCurrents();
		if (PointReached) {
			currentPosition = getCurrentPoint(teachTarget);
			externalEvent = KinovaFSM::PointReached;
			ALL_LOG(logDEBUG) << "Point " << currentPosition << " of Target: " << teachTarget << " is reached.";
		} else {
			//Move to Point
			float fingers[3];
			try {
				std::array<float, 6> coordinatesData = targetCoordinates;
				arm->set_target_cart(coordinatesData.data(), fingers);
			} catch (KinDrv::KinDrvException const &e) {
				error("moveToPoint", e, false);
			}
		}
	} else {
		ALL_LOG(logWARNING) << "moveToPoint: No such Point in Sequence.";
		externalEvent = KinovaFSM::PointNotReached;
	}
}

/*moves Arm to origin of current TeachTarget*/
void KinovaArm::moveToOrigin() {
	float currentCoordinates[6];
	Kinova::Coordinates currentCoordinate{};
	currentPosition = 0;

	getPosition(currentCoordinates);

	//Check if Origin is defined
	if (PositionHandler.hasOrigin(teachTarget)) {
		auto const targetCoordinates = PositionHandler.getOrigin(teachTarget);
		//Check if in range
		bool PointReached = checkIfReached(currentCoordinates);
		checkCurrents();
		if (PointReached) {
			currentPosition = -1;
			externalEvent = KinovaFSM::PointReached;
			ALL_LOG(logDEBUG) << "Origin of " << teachTarget << " reached.";
		} else {
			//Move to Point
			float fingers[3];
			try {
				std::array<float, 6> targetForArm = targetCoordinates;
				arm->set_target_cart(targetForArm.data(), fingers);
			} catch (KinDrv::KinDrvException const &e) {
				error("moveToPoint", e, false);
			}
		}
	} else {
		ALL_LOG(logWARNING) << "moveToOrigin: No Origin defined.";
		externalEvent = KinovaFSM::PointNotReached;
	}
}

/*Saves current Arm-Position to current SequencePoint of teachTarget*/
void KinovaArm::savePoint(int EventVariable) {
	std::array<float, 6> currentCoordinates{};

	if (EventVariable == PositionHandler.getSequence(teachTarget)) {
		getPosition(currentCoordinates.data());

		PositionHandler.savePoint(Kinova::Coordinates{currentCoordinates}, teachTarget);
		PositionHandler.writeToFile();
		externalEvent = KinovaFSM::PointSaved;
	}
	externalEvent = KinovaFSM::PointSaved;
}

/*save current Arm-Position as new origin of Coordinate System*/
void KinovaArm::saveOrigin() {
	std::array<float, 6> currentCoordinates{};

	getPosition(currentCoordinates.data());

	PositionHandler.saveOrigin(Kinova::Coordinates{currentCoordinates}, teachTarget);
	PositionHandler.writeToFile();
	externalEvent = KinovaFSM::OriginSaved;

}

void KinovaArm::deletePoint() {
	PositionHandler.deletePoint(teachTarget);
	PositionHandler.writeToFile();
	externalEvent = KinovaFSM::PointDeleted;
}

/*decrements Sequence if EventVariable is the expected point after decrement.*/
void KinovaArm::previousPoint(int EventVariable) {
	int currentSequence = PositionHandler.getSequence(teachTarget);
	if (EventVariable == currentSequence - 1) {
		PositionHandler.decrementSequence(teachTarget);
		externalEvent = KinovaFSM::PreviousPointSet;
	} else if (EventVariable == currentSequence) {
		externalEvent = KinovaFSM::PreviousPointSet;
	} else {
		externalEvent = KinovaFSM::PreviousPointNotSet;
	}
}

/*increments Sequence if EventVariable is the expected point after increment.*/
void KinovaArm::nextPoint(int EventVariable) {
	int currentSequence = PositionHandler.getSequence(teachTarget);
	if (EventVariable == currentSequence + 1) {
		PositionHandler.incrementSequence(teachTarget);
		externalEvent = KinovaFSM::NextPointSet;
	} else if (EventVariable == currentSequence) {
		externalEvent = KinovaFSM::NextPointSet;
	} else {
		externalEvent = KinovaFSM::NextPointNotSet;
	}
}

/*PrintCurrents if greater than 2. returns True if Currents greater than 3.5, sends Error if Current is greater than 4.5 (fuse at 5 A)*/
auto KinovaArm::checkCurrents() -> bool {
	try {
		KinDrv::jaco_position_t current = arm->get_ang_current();
		float sumCurrent = 0;
		for (int i = 0; i < 6; i++) {
			sumCurrent += current.joints[i];
		}
		if (sumCurrent > 2) {
			ALL_LOG(logDEBUG2) << "getCurrents: " << "(" << current.joints[0] << ", " << current.joints[1] << ", " << current.joints[2] << ", "
					<< current.joints[3] << ", " << current.joints[4] << ", " << current.joints[5] << ")";
		}
		if (sumCurrent > 4.5) {
			error("checkCurrents", "Overcurrent");
			connected = false;
			Error = true;
		}
		if (sumCurrent > 3.5) {
			return true;
		}
	} catch (KinDrv::KinDrvException &e) {
		error("checkCurrents", e, false);
	}
	return false;
}

/*Set-Functinons*/
void KinovaArm::setJoystick(int x, int y, int z) {
	ALL_LOG(logDEBUG4) << "KinovaArm::setJoystick (" << x << ", " << y << ", " << z << ")";
	joystickX = x;
	joystickY = y;
	joystickZ = z;
}

/*Get-Functions*/
auto KinovaArm::getError() -> bool {
	return Error;
}
auto KinovaArm::getInitialize() -> bool {
	return initialized;
}
auto KinovaArm::getActive() -> bool{
	return active;
}
auto KinovaArm::getMode() -> int {
	return mode;
}
auto KinovaArm::getCurrentPosition() -> int {
	return currentPosition;
}
auto KinovaArm::getCurrentPoint() -> int {
	return getCurrentPoint(teachTarget);
}
auto KinovaArm::getCurrentPoint(Kinova::Objective target) -> int {
	return PositionHandler.getSequence(target);
}

auto KinovaArm::getExternalEvent() -> KinovaFSM::Event {
	ALL_LOG(logDEBUG4) << "KinovaArm::getExternalEvent: " << KinovaFSM::eventNames[externalEvent];
	KinovaFSM::Event e = externalEvent;
	externalEvent = KinovaFSM::NoEvent;
	return e;
}

auto KinovaArm::getInternalEvent() -> KinovaFSM::Event {
	ALL_LOG(logDEBUG4) << "KinovaArm::getInternalEvent: " << KinovaFSM::eventNames[externalEvent];
	KinovaFSM::Event e = internalEvent;
	internalEvent = KinovaFSM::NoEvent;
	return e;
}

void KinovaArm::getPosition(float *coordinates) {
	if (!kinovaDummy) {
		KinDrv::jaco_position_t Position;
		try {
			Position = arm->get_cart_pos();
		} catch (KinDrv::KinDrvException const &e) {
			error("getPosition", e, false);
		}

		//transform jaco_position_t to float[]
		for (int i = 0; i < 6; i++) {
			if (i < 3)
				coordinates[i] = Position.position[i];
			else
				coordinates[i] = Position.rotation[i - 3];
		}
	}
	ALL_LOG(logDEBUG4) << "KinovaArm::getPosition: currentCoordinates: (" << coordinates[0] << ", " << coordinates[1] << ", " << coordinates[2]
			<< ", " << coordinates[3] << ", " << coordinates[4] << ", " << coordinates[5] << ")";
}

void KinovaArm::setActive() {
	active = true;
}
void KinovaArm::setInactive() {
	active = false;
	mode = KinovaStatus::NoMode;
}

/*Returns true if arm isn't moving anymore. Starts after initial timer of 500ms to wait for arm to start moving.*/
auto KinovaArm::checkIfReached(float *currentCoordinates) -> bool {
	if (!moveTimerStart) {
		moveTimerStart = std::chrono::steady_clock::now();
	}

	if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - *moveTimerStart) > std::chrono::milliseconds { 500 }) {
		bool pointReached = true;
		ALL_LOG(logDEBUG4) << "KinovaArm::RangeCheck: following Axis do not pass:";
		for (int i = 0; i < 6; i++) {
			float dVel = fabs(currentCoordinates[i] - lastCoordinates[i]);
			if (dVel > velocityRange) {
				ALL_LOG(logDEBUG4) << "[" << i << "]: dVel: " << dVel;
				pointReached = false;
			}
			lastCoordinates[i] = currentCoordinates[i];
		}
		//Returns PointReached if PointReachedCount is higher than 3, to prevent errors.
		if (pointReached) {
			++pointReachedCount;
		} else {
			pointReachedCount = 0;
		}
		if (pointReachedCount > 3) {
			pointReachedCount = 0;
			moveTimerStart = std::experimental::nullopt;
			return pointReached;
		}
	}
	return false;
}

/*PrintForces. TODO: return Forces */
void KinovaArm::getForces() {
	try {
		KinDrv::jaco_position_t force = arm->get_ang_force();
		ALL_LOG(logDEBUG2) << "getForces:" << "(" << force.joints[0] << ", " << force.joints[1] << ", " << force.joints[2] << ", " << force.joints[3]
				<< ", " << force.joints[4] << ", " << force.joints[5] << ")";
	} catch (KinDrv::KinDrvException const &e) {
		error("getForces", e, false);
	}
}
