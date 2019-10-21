#ifndef KINOVA_OBJECTIVES_FILE_HANDLER_H_
#define KINOVA_OBJECTIVES_FILE_HANDLER_H_

#include "MIS_Objective.h"

#include "nlohmann/json.hpp"

using nlohmann::json;

namespace Constants {
	/*
	 * JSON keys
	 */
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

namespace Kinova {

std::vector<Objective> objectivesFromFile(std::istream & ifs) {
    json j;
    ifs >> j;
    
    return 
}

void to_json(json& j, const Objective& o) {

    j = json{
            {Constants::OBJ_NAME, o.name},
            {Constants::OBJ_ORIGIN_ABS, o.is_absolute},
            {Constants::OBJ_ORIGIN, o.origin},
            {Constants::OBJ_SEQUENCE, o.sequence_points}
            };

}

void to_json(json& j, const Coordinates& c) {
    j = json{
            {Constants::OBJ_CSYS_X, c.x},
            {Constants::OBJ_CSYS_Y, c.y},
            {Constants::OBJ_CSYS_Z, c.z},
            {Constants::OBJ_CSYS_PITCH, c.pitch},
            {Constants::OBJ_CSYS_YAW, c.yaw},
            {Constants::OBJ_CSYS_ROLL, c.roll}
    };
}

void from_json(const json& j, Objective& o) {

    j.at(Constants::OBJ_NAME).get_to(o.name);
    j.at(Constants::OBJ_ORIGIN_ABS).get_to(o.is_absolute);
    j.at(Constants::OBJ_ORIGIN).get_to(o.origin);
    j.at(Constants::OBJ_SEQUENCE).get_to(o.sequence_points);

}

void from_json(const json& j, Coordinates& c) {
    j.at(Constants::OBJ_CSYS_X).get_to(c.x);
    j.at(Constants::OBJ_CSYS_Y).get_to(c.y);
    j.at(Constants::OBJ_CSYS_Z).get_to(c.z);
    j.at(Constants::OBJ_CSYS_PITCH).get_to(c.pitch);
    j.at(Constants::OBJ_CSYS_YAW).get_to(c.yaw);
    j.at(Constants::OBJ_CSYS_ROLL).get_to(c.roll);
}

} // namespace Kinova

#endif /* KINOVA_OBJECTIVES_FILE_HANDLER_H_ */