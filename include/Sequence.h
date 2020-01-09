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

	/**
	 * Get the coordinates at the current sequence index relative to absolute zero
	 */
	auto currentPoint() const -> Coordinates;

	/**
	 * Get the coordinates at the current sequence index relative to the given origin
	 */
	auto currentPoint(Origin origin) const -> Coordinates;

	/**
	 * Put the given point into this sequence.
	 *
	 * The point is assumed to be relative to the given origin. The point will be inserted as follows:
	 *
	 *   - if the current sequence has ended, the point will be appended, otherwise
	 *   - if the current sequence has been rewound before its start, the point will be prepended, otherwise
	 *   - the current point will be replaced
	 */
	auto putPoint(Origin origin, Coordinates coordinates) -> bool;

	/**
	 * Remove the current sequence point.
	 *
	 * If the sequence has ended or been rewound before its beginning, this function has no effect.
	 */
	auto removeCurrentPoint() -> void;

	/**
	 * Check if the end of this sequence has been reached.
	 */
	auto endReached() const -> bool;

	/**
	 * Get the length of this sequence
	 */
	auto length() const -> std::size_t;

	/**
	 * Get the current step within this sequence
	 */
	auto currentStep() const -> std::size_t;

	/**
	 * Advance this sequence on step forward
	 */
	void advance();

	/**
	 * Rewind this sequence to the previous step
	 */
	void stepBack();

	/**
	 * Reset this sequence to its first step
	 */
	void reset();

  private:
	friend auto to_json(nlohmann::json &json, Sequence const &sequence) -> void;

	void throwIfEndReached() const;

	std::vector<Coordinates> points;
	Logging::Logger logger;
	int currentIndex = 0;
};

auto to_json(nlohmann::json &json, Sequence const &sequence) -> void;

} // namespace Kinova

#endif /* KINOVA_SEQUENCE_H_ */
