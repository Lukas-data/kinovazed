#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include <array>
#include <string>

namespace Kinova {

namespace JSON_KEY {
	auto constexpr OBJ_NAME = "name";
	auto constexpr OBJ_ORIGIN_ABS = "is_abs";
	auto constexpr OBJ_ORIGIN = "origin";
	auto constexpr OBJ_SEQUENCE = "sequence";
	auto constexpr OBJ_CSYS_X = "X";
	auto constexpr OBJ_CSYS_Y = "Y";
	auto constexpr OBJ_CSYS_Z = "Z";
	auto constexpr OBJ_CSYS_PITCH = "pitch";
	auto constexpr OBJ_CSYS_YAW = "yaw";
	auto constexpr OBJ_CSYS_ROLL = "roll";
}

// objective model
struct Objective {


private:
	std::string name;
	bool is_absolute;
	Coordinates origin;
	std::vector<Coordinates> sequence;
};

// origin model
struct m_origin {
	Coordinates point;
};

struct m_sequence {
	std::vector<Coordinates> seqence_points;
};




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

inline auto isValidObjective(Objective objective) -> bool {
	return objective >= NoObjective && objective <= AntennaPull;
}

}


#endif /* KINOVA_OBJECTIVE_H_ */
