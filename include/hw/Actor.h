#ifndef INCLUDE_HW_ACTOR_H_
#define INCLUDE_HW_ACTOR_H_

#include "hw/Coordinates.h"
#include "support/ToString.h"

#include <atomic>
#include <future>
#include <optional>
#include <set>

namespace KinovaZED::Hw {

enum struct SteeringMode {
	NoMode,
	XYZ,
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
	auto connect() -> bool;

	/**
	 * Disconnect from the actor
	 */
	auto disconnect() -> void;

	/**
	 * Take control over the actor
	 */
	auto takeControl() -> bool;

	/**
	 * Release control over the actor
	 */
	auto releaseControl() -> bool;

	/**
	 * Initialize the actor for operation
	 */
	auto initialize() -> void;

	/**
	 * Stop all ongoing movements and erase all stored trajectories if any are present
	 */
	auto stopMoving() -> bool;

	/**
	 * Move the arm into its home position
	 */
	auto home() -> void;

	/**
	 * Retract the arm into its resting position
	 */
	auto retract() -> void;

	/**
	 * Move to the given position
	 */
	auto moveTo(Coordinates position) -> void;

	/**
	 * Set the joystick input
	 */
	auto setJoystick(int x, int y, int z) -> void;

	/**
	 * Set the current steering mode
	 */
	auto setSteeringMode(SteeringMode mode) -> bool;

	/**
	 * Check if the arm has failed
	 */
	auto hasFailed() const -> bool;

	/**
	 * Get the current position of the actor
	 */
	auto getPosition() const -> Coordinates;

	/**
	 * Get the current steering mode
	 */
	auto getSteeringMode() const -> std::optional<SteeringMode>;

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
	auto virtual doConnect(std::promise<bool> token) -> void = 0;
	auto virtual doDisconnect(std::promise<void> token) -> void = 0;
	auto virtual doTakeControl(std::promise<bool> token) -> void = 0;
	auto virtual doReleaseControl(std::promise<bool> token) -> void = 0;
	auto virtual doInitialize(std::promise<void> token) -> void = 0;
	auto virtual doStopMoving(std::promise<bool> token) -> void = 0;
	auto virtual doHome(std::promise<void> token) -> void = 0;
	auto virtual doRetract(std::promise<void> token) -> void = 0;
	auto virtual doMoveTo(Coordinates position, std::promise<void> token) -> void = 0;
	auto virtual doSetJoystick(int x, int y, int z, std::promise<void> token) -> void = 0;
	auto virtual doSetSteeringMode(SteeringMode mode, std::promise<bool> token) -> void = 0;
	auto virtual doHasFailed(std::promise<bool> token) const -> void = 0;
	auto virtual doGetPosition(std::promise<Coordinates> token) const -> void = 0;
	auto virtual doGetSteeringMode(std::promise<std::optional<SteeringMode>> token) const -> void = 0;

	std::atomic_bool reconnectOnError{false};
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