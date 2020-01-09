#ifndef KINOVA_OBJECTIVE_ID_H_
#define KINOVA_OBJECTIVE_ID_H_

#include <cstdint>
#include <string>

namespace Kinova {

/**
 * The IDs of all known objectives
 */
enum struct ObjectiveId : std::uint8_t {
	None,
	Home,
	Bell,
	Handle,
	OpenDoor,
	PullDoor,
	PlaceCup,
	Antenna,
	AntennaPull,
	INVALID_ID,
};

/**
 * The number of known objectives
 */
auto constexpr static NumberOfObjectives = static_cast<int>(ObjectiveId::INVALID_ID);

/**
 * Convert an objective ID into its string representation
 */
auto getObjectiveName(ObjectiveId id) -> std::string;

/**
 * Convert an objective name into its corresponding ID
 *
 * @throw std::invalid_argument If the name is not associated with any ID
 */
auto getObjectiveId(std::string name) -> ObjectiveId;

/**
 * Check to see if a given int maps to a known objective ID
 */
auto isKnownObjective(int candidate) -> bool;

} // namespace Kinova

#endif