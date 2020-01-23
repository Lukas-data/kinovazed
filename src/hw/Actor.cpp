#include "hw/Actor.h"

#include <algorithm>
#include <iterator>

namespace KinovaZED::Hw {

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

auto isKnownSteeringMode(int candidate) -> bool {
	return candidate >= static_cast<int>(SteeringMode::NoMode) &&
	    candidate < static_cast<int>(SteeringMode::END_OF_ENUM);
}

} // namespace KinovaZED::Hw