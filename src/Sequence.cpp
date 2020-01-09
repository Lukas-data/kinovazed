#include "Sequence.h"

#include "Coordinates.h"
#include "Matrix.h"
#include "spdlog/spdlog.h"

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Kinova {

Sequence::Sequence(Logging::Logger logger)
    : logger{logger} {
}

Sequence::Sequence(std::vector<Coordinates> points, Logging::Logger logger)
    : points{std::move(points)}
    , logger{logger} {
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

auto Sequence::getTransformedCoordinates(Origin origin) const -> Coordinates {
	auto currentCoordinates = getCurrentCoordinates();
	return coordTransform(currentCoordinates, origin.getTransformationMatrix());
}

auto Sequence::getInvertedTransformedCoordinates(Origin origin) const -> Coordinates {
	auto currentCoordinates = getCurrentCoordinates();
	return coordTransform(currentCoordinates, origin.getInvertedTransformationMatrix());
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

auto to_json(nlohmann::json &json, Sequence const &sequence) -> void {
	json = nlohmann::json{sequence.points};
}


auto Sequence::savePoint(Origin origin, Coordinates coordinates) -> bool {
	if (currentPoint > static_cast<decltype(currentPoint)>(numberOfPoints()) && currentPoint != -1) {
		return false;
	}
	auto const transformedCoordinates = coordTransform(coordinates, origin.getInvertedTransformationMatrix());
	if (endReached()) {
		points.push_back(transformedCoordinates);
	} else if (currentPoint == -1) {
		logger->info("Inserting coordinates at [{0}]", currentPoint);
		points.insert(points.begin(), transformedCoordinates);
	} else if (currentPoint >= 0) {
		points[currentPoint] = transformedCoordinates;
	}
	return true;
}

void Sequence::deletePoint() {
	if (currentPoint >= 0 && currentPoint < static_cast<decltype(currentPoint)>(numberOfPoints())) {
		points.erase(points.begin() + currentPoint);
	}
}

// auto Sequence::getPoints() const -> std::vector<Coordinates> {
// 	return points;
// }

} // namespace Kinova
