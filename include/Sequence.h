#ifndef KINOVA_SEQUENCE_H_
#define KINOVA_SEQUENCE_H_

#include "Coordinates.h"
#include "Objective.h"

#include <cstddef>
#include <vector>

namespace Kinova {

using f2d_vec_t = std::vector<std::vector<float>>;

struct Origin {
	Origin() = default;
	explicit Origin(Coordinates const & origin);

	auto getCoordinates() const -> Coordinates const &;
	auto getTransformationMatrix() const -> f2d_vec_t const &;
	auto getInvertedTransformationMatrix() const -> f2d_vec_t const &;

private:
	Coordinates origin;
	f2d_vec_t transformationMatrix;
	f2d_vec_t invertedTransformationMatrix;
};

struct Sequence {
	Sequence() = default;
	Sequence(Coordinates origin, std::vector<Coordinates> points);

	auto getCurrentCoordinates() const -> Coordinates;
	auto getTransformedCoordinates() const -> Coordinates;
	auto getOrigin() const -> Coordinates;
	auto endReached() const -> bool;
	auto numberOfPoints() const -> std::size_t;
	void nextPoint();
	void previousPoint();
	void reset();

private:
	void throwIfEndReached() const;

	Origin origin;
	std::vector<Coordinates> points;
	std::size_t currentPoint = 0;
};

}

#endif /* KINOVA_SEQUENCE_H_ */
