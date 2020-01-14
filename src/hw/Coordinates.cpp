#include "hw/Coordinates.h"

#include "support/Constants.h"

#include <nlohmann/json.hpp>

namespace KinovaZED::Hw {

void to_json(nlohmann::json &j, Coordinates const &c) {
	j = nlohmann::json{{OBJ_CSYS_X, c.x},
	                   {OBJ_CSYS_Y, c.y},
	                   {OBJ_CSYS_Z, c.z},
	                   {OBJ_CSYS_PITCH, c.pitch},
	                   {OBJ_CSYS_YAW, c.yaw},
	                   {OBJ_CSYS_ROLL, c.roll}};
}

void from_json(nlohmann::json const &j, Coordinates &c) {
	j.at(OBJ_CSYS_X).get_to(c.x);
	j.at(OBJ_CSYS_Y).get_to(c.y);
	j.at(OBJ_CSYS_Z).get_to(c.z);
	j.at(OBJ_CSYS_PITCH).get_to(c.pitch);
	j.at(OBJ_CSYS_YAW).get_to(c.yaw);
	j.at(OBJ_CSYS_ROLL).get_to(c.roll);
}

} // namespace KinovaZED::Hw
