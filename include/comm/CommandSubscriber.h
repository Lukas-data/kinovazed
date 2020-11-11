#ifndef INCLUDE_COMM_COMMAND_SUBSCRIBER_H
#define INCLUDE_COMM_COMMAND_SUBSCRIBER_H

#include "comm/Command.h"

namespace KinovaZED::Comm {

struct CommandSubscriber {
	auto virtual process(Command command) -> void {
		(void)command;
	}

	auto virtual onInterfaceStarted() -> void {
	}

	auto virtual onInterfaceStopped() -> void {
	}
};

} // namespace KinovaZED::Comm


#endif