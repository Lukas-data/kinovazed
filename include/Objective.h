#ifndef KINOVA_OBJECTIVE_H_
#define KINOVA_OBJECTIVE_H_

#include "Coordinates.h"
#include "Logging.h"
#include "ObjectiveId.h"
#include "Sequence.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <type_traits>

namespace Kinova {

struct Objective {
	explicit Objective(Logging::Logger logger)
	    : id{ObjectiveId::None}
	    , origin{}
	    , sequence{logger}
	    , absolute{} {
	}

	Objective(nlohmann::json const &json, Logging::Logger logger);

	auto getSequence() -> Sequence &;

	auto getSequence() const -> Sequence const &;

	auto hasOrigin() const -> bool;

	auto getOrigin() const -> Coordinates;

	auto isAbsolute() const -> bool;

	auto getId() const -> ObjectiveId;

  private:
	friend auto to_json(nlohmann::json &output, Objective const &objective) -> void;

	ObjectiveId id;
	Coordinates origin;
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
