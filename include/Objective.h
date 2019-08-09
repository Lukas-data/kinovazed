#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include <array>
#include <string>

namespace Kinova {

static constexpr int NumberOfObjectives = 8; //excl. NoObjective

enum Objective {
	NoObjective, Home, Bell, Handle, OpenDoor, PullDoor, PlaceCup, Antenna, AntennaPull
};

constexpr std::array<char const *, NumberOfObjectives + 1> ObjectiveNames{
	"NoObjective", "Home", "Bell", "Handle", "OpenDoor", "PullDoor", "PlaceCup", "Antenna", "AntennaPull"
};

inline std::string getObjectiveName(Objective objective) {
	if (objective > NumberOfObjectives) {
		return "Unknown";
	}
	return ObjectiveNames[objective];
}

}


#endif /* KINOVA_OBJECTIVE_H_ */
