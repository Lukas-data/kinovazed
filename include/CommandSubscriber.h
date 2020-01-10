#ifndef INCLUDE_COMMAND_SUBSCRIBER_H
#define INCLUDE_COMMAND_SUBSCRIBER_H

#include "Command.h"

namespace Kinova {

struct CommandSubscriber {
	auto virtual process(Command command) -> void = 0;

	auto virtual onInterfaceStarted() -> void = 0;

	auto virtual onInterfaceStopped() -> void = 0;
};

} // namespace Kinova


#endif