#include "Sequence.h"
#include "Coordinates.h"
#include "Matrix.h"

#include "spdlog/spdlog.h"

#include <array>
#include <cmath>
#include <cstddef>

#include <stdexcept>
#include <utility>
#include <vector>

namespace Kinova {

static auto calculateTransformationMatrix(Coordinates const & origin) -> f2d_vec_t {
	std::array<float, 3> const components{origin.x, origin.y, origin.z};
	std::array<float, 3> const cosines{std::cos(origin.pitch), std::cos(origin.yaw), std::cos(origin.roll)};
	std::array<float, 3> const sines{std::sin(origin.pitch), std::sin(origin.yaw), std::sin(origin.roll)};

	return {
		{cosines[1] * cosines[2], 									-cosines[1] * sines[2],										sines[1], 					components[0]},
		{cosines[0] * sines[2] + sines[0] * cosines[2] * sines[1],	cosines[0] * cosines[2] - sines[0] * sines[1] * sines[2],	-sines[0] * cosines[1], 	components[1]},
		{sines[0] * sines[2] - cosines[0] * cosines[2] * sines[1],	sines[0] * cosines[2] + cosines[0] * sines[1] * sines[2], 	cosines[0] * cosines[1],	components[2]},
		{0,															0,															0,							1}
	};
}

static auto invertMatrix(f2d_vec_t const & matrix) -> f2d_vec_t {
	return {
		{matrix[0][0],	matrix[1][0], 	matrix[2][0],	-matrix[0][0] * matrix[0][3] - matrix[1][3] * matrix[1][0] - matrix[2][3] * matrix[2][0]},
		{matrix[0][1],	matrix[1][1], 	matrix[2][1],	-matrix[0][1] * matrix[0][3] - matrix[1][3] * matrix[1][1] - matrix[2][3] * matrix[2][1]},
		{matrix[0][2],	matrix[1][2], 	matrix[2][2],	-matrix[0][2] * matrix[0][3] - matrix[1][3] * matrix[1][2] - matrix[2][3] * matrix[2][2]},
		{0,				0,				0,				1}
	};
}

Origin::Origin() : Origin{Coordinates{}}{}

Origin::Origin(Coordinates const & origin) : origin{origin}, transformationMatrix{calculateTransformationMatrix(origin)}, invertedTransformationMatrix{invertMatrix(transformationMatrix)} {
}

auto Origin::getCoordinates() const -> Coordinates const & {
	return origin;
}

auto Origin::getTransformationMatrix() const -> f2d_vec_t const & {
	return transformationMatrix;
}

auto Origin::getInvertedTransformationMatrix() const -> f2d_vec_t const & {
	return invertedTransformationMatrix;
}

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

auto Sequence::getTransformedCoordinates() const -> Coordinates {
	auto currentCoordinates = getCurrentCoordinates();
	return coordTransform(currentCoordinates, origin.getTransformationMatrix());
}

auto Sequence::getInvertedTransformedCoordinates() const -> Coordinates {
	auto currentCoordinates = getCurrentCoordinates();
	return coordTransform(currentCoordinates, origin.getInvertedTransformationMatrix());
}

auto Sequence::getOrigin() const -> Coordinates {
	return origin.getCoordinates();
}

void Sequence::setOrigin(Coordinates newOrigin) {
	origin = Origin{newOrigin};
}

void Sequence::resetOrigin() {
	origin = Origin{};
}

auto Sequence::endReached() const -> bool {
	return currentPoint >= static_cast<int>(points.size());
}

auto Sequence::numberOfPoints() const -> std::size_t {
	return points.size();
}

auto Sequence::currentSequencePoint() const -> std::size_t {
	return currentPoint;
}

void Sequence::nextPoint() {
//	throwIfEndReached();
	currentPoint++;
}

void Sequence::previousPoint() {
//	if (currentPoint == 0) {
//		throw std::logic_error{"Cannot go to previous point from the beginning"};
//	}
	currentPoint--;
}

void Sequence::reset() {
	currentPoint = 0;
}


auto Sequence::savePoint(Coordinates coordinates) -> bool {
	if (currentPoint > static_cast<decltype(currentPoint)>(numberOfPoints()) && currentPoint != -1) {
		return false;
	}
	auto const transformedCoordinates = coordTransform(coordinates, origin.getInvertedTransformationMatrix());
	if (endReached()) {
		points.push_back(transformedCoordinates);
	} else if (currentPoint == -1) {
		spdlog::info("Inserting coordinates at [{0}]", currentPoint);
		points.insert(points.begin(), transformedCoordinates);
	} else if (currentPoint >= 0) {
		points[currentPoint] = transformedCoordinates;
	}
	return true;
}

void Sequence::deletePoint() {
	if (currentPoint >= 0 && currentPoint < static_cast<decltype(currentPoint)>(numberOfPoints())){
		points.erase(points.begin() + currentPoint);
	}
}

}
