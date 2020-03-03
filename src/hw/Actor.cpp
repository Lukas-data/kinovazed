#include "hw/Actor.h"

#include "support/EnumUtils.h"
#include "support/ToString.h"

#include <algorithm>
#include <cassert>
#include <future>
#include <iterator>

namespace KinovaZED::Hw {

auto constexpr modeNames = std::array{
    std::pair{SteeringMode::NoMode, "NoMode"},
    std::pair{SteeringMode::XYZ, "XYZ"},
    std::pair{SteeringMode::Rotation, "Rotation"},
    std::pair{SteeringMode::Axis1to3, "Axis1to3"},
    std::pair{SteeringMode::Axis4to6, "Axis4to6"},
    std::pair{SteeringMode::Freeze, "Freeze"},
};

static_assert(enumNameMappingsAreUnique(modeNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(modeNames, SteeringMode::NoMode), "Missing entry in name map!");


auto isKnownSteeringMode(int candidate) -> bool {
	return candidate >= static_cast<int>(SteeringMode::NoMode) &&
	    candidate < static_cast<int>(SteeringMode::END_OF_ENUM);
}

auto isKnownSteeringMode(std::string const &candidate) -> bool {
	auto found =
	    std::find_if(cbegin(modeNames), cend(modeNames), [&](auto entry) { return entry.second == candidate; });
	return found != cend(modeNames);
}

auto Actor::connect() -> bool {
	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doConnect(std::move(token));
	return future.get();
}

auto Actor::disconnect() -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doDisconnect(std::move(token));
	return future.get();
}

auto Actor::takeControl() -> bool {
	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doTakeControl(std::move(token));
	return future.get();
}

auto Actor::releaseControl() -> bool {
	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doReleaseControl(std::move(token));
	return future.get();
}

auto Actor::initialize() -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doInitialize(std::move(token));
	return future.get();
}

auto Actor::stopMoving() -> bool {
	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doStopMoving(std::move(token));
	return future.get();
}

auto Actor::home() -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doHome(std::move(token));
	return future.get();
}

auto Actor::retract() -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doRetract(std::move(token));
	return future.get();
}

auto Actor::moveTo(Coordinates position) -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doMoveTo(std::move(position), std::move(token));
	return future.get();
}

auto Actor::setJoystick(int x, int y, int z) -> void {
	auto token = std::promise<void>{};
	auto future = token.get_future();
	doSetJoystick(x, y, z, std::move(token));
	return future.get();
}

auto Actor::setSteeringMode(SteeringMode mode) -> bool {
	assert(isKnownSteeringMode(static_cast<int>(mode)));

	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doSetSteeringMode(mode, std::move(token));
	return future.get();
}

auto Actor::hasFailed() const -> bool {
	auto token = std::promise<bool>{};
	auto future = token.get_future();
	doHasFailed(std::move(token));
	return future.get();
}

auto Actor::getPosition() const -> Coordinates {
	auto token = std::promise<Coordinates>{};
	auto future = token.get_future();
	doGetPosition(std::move(token));
	return future.get();
}

auto Actor::getSteeringMode() const -> std::optional<SteeringMode> {
	auto token = std::promise<std::optional<SteeringMode>>{};
	auto future = token.get_future();
	doGetSteeringMode(std::move(token));
	return future.get();
}

auto Actor::setShouldReconnectOnError(bool reconnect) -> void {
	reconnectOnError = reconnect;
}

auto Actor::shouldReconnectOnError() -> bool {
	return reconnectOnError;
}

auto Actor::subscribe(EventSubscriberPtr subscriber) -> bool {
	return eventSubscribers.insert(subscriber).second;
}

auto Actor::unsubscribe(EventSubscriberPtr subscriber) -> bool {
	return static_cast<bool>(eventSubscribers.erase(subscriber));
}

auto Actor::firePositionReached(Coordinates point) -> void {
	for_each(begin(eventSubscribers), end(eventSubscribers), [&](auto subscriber) {
		subscriber->onPositionReached(*this, point);
	});
}

auto Actor::fireHomeReached() -> void {
	for_each(
	    begin(eventSubscribers), end(eventSubscribers), [this](auto subscriber) { subscriber->onHomeReached(*this); });
}

auto Actor::fireRetractionPointReached() -> void {
	for_each(begin(eventSubscribers), end(eventSubscribers), [this](auto subscriber) {
		subscriber->onRetractionPointReached(*this);
	});
}

auto Actor::fireSteeringModeChanged(SteeringMode mode) -> void {
	for_each(begin(eventSubscribers), end(eventSubscribers), [&](auto subscriber) {
		subscriber->onSteeringModeChanged(*this, mode);
	});
}

auto Actor::fireReconnectedDueToError() -> void {
	for_each(begin(eventSubscribers), end(eventSubscribers), [this](auto subscriber) {
		subscriber->onReconnectedDueToError(*this);
	});
}

auto Actor::fireInitializationFinished() -> void {
	for_each(begin(eventSubscribers), end(eventSubscribers), [this](auto subscriber) {
		subscriber->onInitializationFinished(*this);
	});
}

} // namespace KinovaZED::Hw

namespace KinovaZED {

using namespace Hw;

template<>
auto toString(SteeringMode const &id) -> std::string {
	assert(isKnownSteeringMode(static_cast<std::underlying_type_t<SteeringMode>>(id)));

	auto found = std::find_if(cbegin(modeNames), cend(modeNames), [&](auto entry) { return entry.first == id; });
	return found->second;
}

template<>
auto fromString(std::string const &name) -> SteeringMode {
	auto found = std::find_if(cbegin(modeNames), cend(modeNames), [&](auto entry) { return entry.second == name; });
	assert(found != cend(modeNames));
	return found->first;
}

} // namespace KinovaZED