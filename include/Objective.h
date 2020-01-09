#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include "Coordinates.h"
#include "Logging.h"
#include "ObjectiveId.h"
#include "Origin.h"
#include "Sequence.h"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <type_traits>

namespace Kinova {

struct Objective {
	explicit Objective(Logging::Logger logger);

	Objective(nlohmann::json const &json, Logging::Logger logger);

	auto getOrigin() const -> Origin;
	auto setOrigin(Coordinates point) -> void;

	auto getCurrentSequencePoint() const -> Coordinates;
	auto getTransformedSequencePoint() const -> Coordinates;
	auto saveSequencePoint(Coordinates point) -> bool;
	auto deleteSequencePoint() -> void;

	auto numberOfSequencePoints() const -> std::size_t;
	auto currentSequenceIndex() const -> std::size_t;
	auto forwardSequence() -> void;
	auto rewindSequence() -> void;
	auto resetSequence() -> void;
	auto sequenceEnded() const -> bool;

	auto isAbsolute() const -> bool;

	auto getId() const -> ObjectiveId;

  private:
	friend auto to_json(nlohmann::json &output, Objective const &objective) -> void;

	ObjectiveId id;
	Origin origin;
	Sequence sequence;
	bool absolute;
};

/**
 * Load a list of objectives from the given input stream
 */
auto loadObjectives(std::istream &in, Logging::Logger logger) -> std::vector<Kinova::Objective>;

/**
 * Serialize an objective from a json structure
 */
auto to_json(nlohmann::json &output, Objective const &objective) -> void;

} // namespace Kinova


#endif /* KINOVA_OBJECTIVE_H_ */
