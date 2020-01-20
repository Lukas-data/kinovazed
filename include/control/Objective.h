#ifndef INCLUDE_CONTROL_OBJECTIVE_H_
#define INCLUDE_CONTROL_OBJECTIVE_H_

#include "hw/Coordinates.h"
#include "hw/Origin.h"
#include "support/Logging.h"
#include "support/ToString.h"

#include <nlohmann/json.hpp>

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <optional>
#include <string>
#include <type_traits>

namespace KinovaZED::Control {

struct Objective {

	enum struct Id : std::uint8_t {
		Bell,
		Handle,
		OpenDoor,
		PullDoor,
		PlaceCup,
		Antenna,
		AntennaPull,

		// End Marker
		END_OF_ENUM,
	};

	Objective(nlohmann::json const &json, Logger logger);

	auto getOrigin() const -> Hw::Origin;
	auto setOrigin(Hw::Coordinates point) -> void;

	auto getId() const -> Id;
	auto isAbsolute() const -> bool;

	auto nextPoint() -> std::optional<Hw::Coordinates>;

  private:
	friend auto to_json(nlohmann::json &output, Objective const &objective) -> void;

	Id id;
	Hw::Origin origin;
	std::vector<Hw::Coordinates> sequence;
	bool absolute;

	std::make_signed_t<std::size_t> currentSequenceIndex{-1};
};

/**
 * The number of supported objectives
 */
auto constexpr static NumberOfObjectives = static_cast<int>(Objective::Id::END_OF_ENUM);

/**
 * Check to see if a given int maps to a known objective ID
 */
auto isKnownObjectiveId(int candidate) -> bool;

/**
 * Check to see if a given string maps to a known objective ID
 */
auto isKnownObjectiveId(std::string const &candidate) -> bool;

/**
 * Load a list of objectives from the given input stream
 */
auto loadObjectives(std::istream &in, Logger logger) -> std::vector<Objective>;

/**
 * Serialize an objective from a json structure
 */
auto to_json(nlohmann::json &output, Objective const &objective) -> void;

} // namespace KinovaZED::Control


namespace KinovaZED {

template<>
auto toString(Control::Objective::Id const &) -> std::string;

template<>
auto fromString(std::string const &) -> Control::Objective::Id;

} // namespace KinovaZED

#endif
