#include "hw/Actor.h"

namespace KinovaZED::Hw {

auto Actor::setShouldReconnectOnError(bool reconnect) -> void {
	reconnectOnError = reconnect;
}

auto Actor::shouldReconnectOnError() -> bool {
	return reconnectOnError;
}

auto isKnownSteeringMode(int candidate) -> bool {
	return candidate >= static_cast<int>(SteeringMode::NoMode) &&
	    candidate < static_cast<int>(SteeringMode::END_OF_ENUM);
}

} // namespace KinovaZED::Hw