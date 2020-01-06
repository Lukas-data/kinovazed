#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include "Coordinates.h"
#include "nlohmann/json.hpp"

#include <array>
#include <iosfwd>
#include <string>
#include <vector>

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
} // namespace JSON_KEY

// objective model
struct JSONObjective {
	friend void to_json(nlohmann::json &j, JSONObjective const &o);
	friend void from_json(nlohmann::json const &j, JSONObjective &o);
	friend std::ostream &operator<<(std::ostream &out, JSONObjective const &objective);

  private:
	std::string name;
	bool is_absolute;
	Coordinates origin;
	std::vector<Coordinates> sequence;
};

static constexpr int NumberOfObjectives = 8; // excl. NoObjective

enum Objective { NoObjective, Home, Bell, Handle, OpenDoor, PullDoor, PlaceCup, Antenna, AntennaPull };

constexpr std::array<char const *, NumberOfObjectives + 1> ObjectiveNames{
    "NoObjective", "Home", "Bell", "Handle", "OpenDoor", "PullDoor", "PlaceCup", "Antenna", "AntennaPull"};

inline std::string getObjectiveName(Objective objective) {
	if (objective > NumberOfObjectives) {
		return "Unknown";
	}
	return ObjectiveNames[objective];
}

inline auto isValidObjective(Objective objective) -> bool {
	return objective >= NoObjective && objective <= AntennaPull;
}

} // namespace Kinova


#endif /* KINOVA_OBJECTIVE_H_ */
