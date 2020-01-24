#include "comm/Heartbeat.h"

#include "support/ToString.h"

#include <spdlog/fmt/fmt.h>

#include <bitset>
#include <string>

namespace KinovaZED {

template<>
auto toString(Comm::Heartbeat const &heartbeat) -> std::string {
	return fmt::format("Heartbeat:{}", heartbeat.state.to_ulong());
}

} // namespace KinovaZED