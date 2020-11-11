#include "hw/Coordinates.h"

#include <nlohmann/json.hpp>

namespace KinovaZED::Hw {

auto constexpr jsonKeyX = "X";
auto constexpr jsonKeyY = "Y";
auto constexpr jsonKeyZ = "Z";
auto constexpr jsonKeyPitch = "pitch";
auto constexpr jsonKeyYaw = "yaw";
auto constexpr jsonKeyRoll = "roll";


void to_json(nlohmann::json &j, Coordinates const &c) {
	j = nlohmann::json{{jsonKeyX, c.x},
	                   {jsonKeyY, c.y},
	                   {jsonKeyZ, c.z},
	                   {jsonKeyPitch, c.pitch},
	                   {jsonKeyYaw, c.yaw},
	                   {jsonKeyRoll, c.roll}};
}

void from_json(nlohmann::json const &j, Coordinates &c) {
	j.at(jsonKeyX).get_to(c.x);
	j.at(jsonKeyY).get_to(c.y);
	j.at(jsonKeyZ).get_to(c.z);
	j.at(jsonKeyPitch).get_to(c.pitch);
	j.at(jsonKeyYaw).get_to(c.yaw);
	j.at(jsonKeyRoll).get_to(c.roll);
}

} // namespace KinovaZED::Hw
