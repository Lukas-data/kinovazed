#ifndef KINOVA_SEQUENCE_H_
#define KINOVA_SEQUENCE_H_

#include "Coordinates.h"
#include "Logging.h"
#include "Origin.h"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <vector>

namespace Kinova {

struct Sequence {
	Sequence(Logging::Logger logger);
	Sequence(std::vector<Coordinates> points, Logging::Logger logger);

	auto getCurrentCoordinates() const -> Coordinates;
	auto getTransformedCoordinates(Origin origin) const -> Coordinates;
	auto getInvertedTransformedCoordinates(Origin origin) const -> Coordinates;
	auto savePoint(Origin origin, Coordinates coordinates) -> bool;
	auto deletePoint() -> void;
	auto endReached() const -> bool;
	auto numberOfPoints() const -> std::size_t;
	auto currentSequencePoint() const -> std::size_t;
	void nextPoint();
	void previousPoint();
	void reset();

  private:
	friend auto to_json(nlohmann::json &json, Sequence const &sequence) -> void;

	void throwIfEndReached() const;

	std::vector<Coordinates> points;
	Logging::Logger logger;
	int currentPoint = 0;
};

auto to_json(nlohmann::json &json, Sequence const &sequence) -> void;

} // namespace Kinova

#endif /* KINOVA_SEQUENCE_H_ */
