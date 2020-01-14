#ifndef INCLUDE_COMM_COMMAND_SUBSCRIBER_H
#define INCLUDE_COMM_COMMAND_SUBSCRIBER_H

#include "comm/Command.h"

namespace KinovaZED::Comm {

struct CommandSubscriber {
	auto virtual process(Command command) -> void = 0;

	auto virtual onInterfaceStarted() -> void = 0;

	auto virtual onInterfaceStopped() -> void = 0;
};

} // namespace KinovaZED::Comm


#endif