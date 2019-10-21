#include "Coordinates.h"

#include "Constants.h"

#include "nlohmann/json.hpp"

namespace Kinova {

void to_json(nlohmann::json &j, Coordinates const &c) {
	j = nlohmann::json {
            {Constants::OBJ_CSYS_X, c.x},
            {Constants::OBJ_CSYS_Y, c.y},
            {Constants::OBJ_CSYS_Z, c.z},
            {Constants::OBJ_CSYS_PITCH, c.pitch},
            {Constants::OBJ_CSYS_YAW, c.yaw},
            {Constants::OBJ_CSYS_ROLL, c.roll}
    };
}

void from_json(nlohmann::json const &j, Coordinates &c) {
    j.at(Constants::OBJ_CSYS_X).get_to(c.x);
    j.at(Constants::OBJ_CSYS_Y).get_to(c.y);
    j.at(Constants::OBJ_CSYS_Z).get_to(c.z);
    j.at(Constants::OBJ_CSYS_PITCH).get_to(c.pitch);
    j.at(Constants::OBJ_CSYS_YAW).get_to(c.yaw);
    j.at(Constants::OBJ_CSYS_ROLL).get_to(c.roll);
}

}
