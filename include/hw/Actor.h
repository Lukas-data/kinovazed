#ifndef INCLUDE_HW_ACTOR_H_
#define INCLUDE_HW_ACTOR_H_

#include "hw/Coordinates.h"

namespace KinovaZED::Hw {

enum struct SteeringMode {
	NoMode,
	Translation,
	Rotation,
	Axis1,
	Axis2,
};

auto isKnownSteeringMode(int candidate) -> bool;

struct Actor {
	/**
	 * Connect to the actor
	 */
	auto virtual connect() -> bool = 0;

	/**
	 * Disconnect from the actor
	 */
	auto virtual disconnect() -> void = 0;

	/**
	 * Take control over the actor
	 */
	auto virtual takeControl() -> bool = 0;

	/**
	 * Release control over the actor
	 */
	auto virtual releaseControl() -> bool = 0;

	/**
	 * Stop all ongoing movements and erase all stored trajectories if any are present
	 */
	auto virtual stopMoving() -> bool = 0;

	/**
	 * Move the arm into its home position
	 */
	auto virtual home() -> void = 0;

	/**
	 * Retract the arm into its resting position
	 */
	auto virtual retract() -> void = 0;

	/**
	 * Move to the given position
	 */
	auto virtual moveTo(Coordinates position) -> void = 0;

	/**
	 * Set the joystick input
	 */
	auto virtual setJoystick(int x, int y, int z) -> void = 0;

	/**
	 * Set the current steering mode
	 */
	auto virtual setSteeringMode(SteeringMode mode) -> bool = 0;

	/**
	 * Check if the arm has failed
	 */
	auto virtual hasFailed() const -> bool = 0;

	auto setShouldReconnectOnError(bool reconnect) -> void;

  private:
	bool reconnectOnError{false};
};

} // namespace KinovaZED::Hw

#endif