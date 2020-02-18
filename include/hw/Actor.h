#ifndef INCLUDE_HW_ACTOR_H_
#define INCLUDE_HW_ACTOR_H_

#include "hw/Coordinates.h"
#include "support/ToString.h"

#include <set>

namespace KinovaZED::Hw {

enum struct SteeringMode {
	NoMode,
	Translation,
	Rotation,
	Axis1to3,
	Axis4to6,
	Freeze,

	// End Marker
	END_OF_ENUM
};

auto isKnownSteeringMode(int candidate) -> bool;

auto isKnownSteeringMode(std::string const &candidate) -> bool;

struct Actor {

	struct EventSubscriber {
		auto virtual onPositionReached(Actor &who, Coordinates point) -> void {
			(void)who, (void)point;
		}

		auto virtual onHomeReached(Actor &who) -> void {
			(void)who;
		}

		auto virtual onRetractionPointReached(Actor &who) -> void {
			(void)who;
		}

		auto virtual onSteeringModeChanged(Actor &who, SteeringMode mode) -> void {
			(void)who, (void)mode;
		}

		auto virtual onReconnectedDueToError(Actor &who) -> void {
			(void)who;
		}

		auto virtual onInitializationFinished(Actor &who) -> void {
			(void)who;
		}
	};

	using EventSubscriberPtr = std::shared_ptr<EventSubscriber>;

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
	 * Initialize the actor for operation
	 */
	auto virtual initialize() -> void = 0;

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

	/**
	 * Get the current position of the actor
	 */
	auto virtual getPosition() const -> Coordinates = 0;

	auto setShouldReconnectOnError(bool reconnect) -> void;
	auto shouldReconnectOnError() -> bool;

	auto subscribe(EventSubscriberPtr subscriber) -> bool;
	auto unsubscribe(EventSubscriberPtr subscriber) -> bool;

  protected:
	auto firePositionReached(Coordinates point) -> void;
	auto fireHomeReached() -> void;
	auto fireRetractionPointReached() -> void;
	auto fireSteeringModeChanged(SteeringMode mode) -> void;
	auto fireReconnectedDueToError() -> void;
	auto fireInitializationFinished() -> void;

  private:
	bool reconnectOnError{false};
	std::set<EventSubscriberPtr> eventSubscribers{};
};

} // namespace KinovaZED::Hw

namespace KinovaZED {

template<>
auto toString(Hw::SteeringMode const &) -> std::string;

template<>
auto fromString(std::string const &) -> Hw::SteeringMode;

} // namespace KinovaZED

#endif