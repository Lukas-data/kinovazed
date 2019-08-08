#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_



namespace Kinova {

static constexpr int NumberOfObjectives = 8; //excl. NoObjective
enum Objective {
	NoObjective, Home, Bell, Handle, OpenDoor, PullDoor, PlaceCup, Antenna, AntennaPull
};

}


#endif /* KINOVA_OBJECTIVE_H_ */
