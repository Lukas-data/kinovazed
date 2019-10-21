#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include "Coordinates.h"

#include <array>
#include <string>
#include <cstddef>
#include <vector>

namespace Kinova {

struct Objective {

    std::string name;
	bool is_absolute;
	Coordinates origin;
	std::vector<Coordinates> sequence_points;


private:


    int currentPoint = 0;

};

}

#endif /* KINOVA_OBJECTIVE_H_ */