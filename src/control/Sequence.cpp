#include "control/Sequence.h"

#include "hw/Coordinates.h"
#include "math/Matrix.h"

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

namespace KinovaZED::Control {

Sequence::Sequence(Logger logger)
    : logger{logger} {
}

Sequence::Sequence(std::vector<Hw::Coordinates> points, Logger logger)
    : points{std::move(points)}
    , logger{logger} {
}

void Sequence::throwIfEndReached() const {
	if (endReached()) {
		throw std::logic_error{"End of sequence reached"};
	}
}

auto Sequence::currentPoint() const -> Hw::Coordinates {
	throwIfEndReached();
	return points[currentIndex];
}

auto Sequence::currentPoint(Hw::Origin origin) const -> Hw::Coordinates {
	return coordTransform(currentPoint(), origin.getTransformationMatrix());
}

auto Sequence::endReached() const -> bool {
	return currentIndex >= static_cast<int>(points.size());
}

auto Sequence::length() const -> std::size_t {
	return points.size();
}

auto Sequence::currentStep() const -> std::size_t {
	return currentIndex;
}

void Sequence::advance() {
	//	throwIfEndReached();
	currentIndex++;
}

void Sequence::stepBack() {
	//	if (currentPoint == 0) {
	//		throw std::logic_error{"Cannot go to previous point from the beginning"};
	//	}
	currentIndex--;
}

void Sequence::reset() {
	currentIndex = 0;
}

auto to_json(nlohmann::json &json, Sequence const &sequence) -> void {
	json = nlohmann::json{sequence.points};
}


auto Sequence::putPoint(Hw::Origin origin, Hw::Coordinates coordinates) -> bool {
	if (currentIndex > static_cast<decltype(currentIndex)>(length()) && currentIndex != -1) {
		return false;
	}
	auto const transformedCoordinates = coordTransform(coordinates, origin.getInvertedTransformationMatrix());
	if (endReached()) {
		points.push_back(transformedCoordinates);
	} else if (currentIndex == -1) {
		logger->info("Inserting coordinates at [{0}]", currentIndex);
		points.insert(points.begin(), transformedCoordinates);
	} else if (currentIndex >= 0) {
		points[currentIndex] = transformedCoordinates;
	}
	return true;
}

void Sequence::removeCurrentPoint() {
	if (currentIndex >= 0 && currentIndex < static_cast<decltype(currentIndex)>(length())) {
		points.erase(points.begin() + currentIndex);
	}
}

} // namespace KinovaZED::Control
