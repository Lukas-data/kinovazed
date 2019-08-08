#include "Sequence.h"
#include "Coordinates.h"

#include <stdexcept>

#include <utility>
#include <vector>

namespace Kinova {

Sequence::Sequence(Coordinates origin, std::vector<Coordinates> points) : origin{origin}, points{std::move(points)} {

}

void Sequence::throwIfEndReached() const {
	if (endReached()) {
		throw std::logic_error{"End of sequence reached"};
	}
}

auto Sequence::getCurrentCoordinates() const -> Coordinates {
	throwIfEndReached();
	return points[currentPoint];
}

auto Sequence::getOrigin() const -> Coordinates {
	return origin;
}

auto Sequence::endReached() const -> bool {
	return currentPoint >= points.size();
}

void Sequence::nextPoint() {
	throwIfEndReached();
	currentPoint++;
}

void Sequence::previousPoint() {
	if (currentPoint == 0) {
		throw std::logic_error{"Cannot go to previous point from the beginning"};
	}
	currentPoint--;
}

void Sequence::resetSequence() {
	currentPoint = 0;
}

}
