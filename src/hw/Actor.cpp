#include "hw/Actor.h"

namespace KinovaZED::Hw {

auto Actor::setShouldReconnectOnError(bool reconnect) -> void {
	reconnectOnError = reconnect;
}

} // namespace KinovaZED::Hw