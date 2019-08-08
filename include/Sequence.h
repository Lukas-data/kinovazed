#ifndef KINOVA_SEQUENCE_H_
#define KINOVA_SEQUENCE_H_

#include "Coordinates.h"
#include "Objective.h"

#include <cstddef>
#include <vector>

namespace Kinova {

struct Sequence {

	Sequence(Coordinates origin, std::vector<Coordinates> points);

	auto getCurrentCoordinates() const -> Coordinates;
	auto getOrigin() const -> Coordinates;
	auto endReached() const -> bool;
	void nextPoint();
	void previousPoint();
	void resetSequence();

private:
	void throwIfEndReached() const;

	Coordinates origin;
	std::vector<Coordinates> points;
	std::size_t currentPoint = 0;
};

}

#endif /* KINOVA_SEQUENCE_H_ */
