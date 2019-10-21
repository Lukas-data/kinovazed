#include "Objective.h"

#include "Constants.h"

#include "nlohmann/json.hpp"

namespace Kinova {

void to_json(nlohmann::json & j, JSONObjective const & o) {
	j = nlohmann::json {
            {Constants::OBJ_NAME, o.name},
            {Constants::OBJ_ORIGIN_ABS, o.is_absolute},
            {Constants::OBJ_ORIGIN, o.origin},
            {Constants::OBJ_SEQUENCE, o.sequence}
            };
}

void from_json(nlohmann::json const & j, JSONObjective & o) {
    j.at(Constants::OBJ_NAME).get_to(o.name);
    j.at(Constants::OBJ_ORIGIN_ABS).get_to(o.is_absolute);
    j.at(Constants::OBJ_ORIGIN).get_to(o.origin);
    j.at(Constants::OBJ_SEQUENCE).get_to(o.sequence);
}

std::ostream & operator<<(std::ostream & out, JSONObjective const & objective) {
	return out << nlohmann::json{objective};
}

}
