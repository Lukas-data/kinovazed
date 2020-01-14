#ifndef INCLUDE_CONTROL_OBJECTIVE_H_
#define INCLUDE_CONTROL_OBJECTIVE_H_

#include "control/ObjectiveId.h"
#include "control/Sequence.h"
#include "hw/Coordinates.h"
#include "hw/Origin.h"
#include "support/Logging.h"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <type_traits>

namespace KinovaZED::Control {

struct Objective {
	explicit Objective(Logger logger);

	Objective(nlohmann::json const &json, Logger logger);

	auto getOrigin() const -> Hw::Origin;
	auto setOrigin(Hw::Coordinates point) -> void;

	auto getCurrentSequencePoint() const -> Hw::Coordinates;
	auto getTransformedSequencePoint() const -> Hw::Coordinates;
	auto saveSequencePoint(Hw::Coordinates point) -> bool;
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
	Hw::Origin origin;
	Sequence sequence;
	bool absolute;
};

/**
 * Load a list of objectives from the given input stream
 */
auto loadObjectives(std::istream &in, Logger logger) -> std::vector<Objective>;

/**
 * Serialize an objective from a json structure
 */
auto to_json(nlohmann::json &output, Objective const &objective) -> void;

} // namespace KinovaZED::Control


#endif
