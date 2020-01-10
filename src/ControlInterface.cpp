#include "ControlInterface.h"

#include "Command.h"
#include "CommandSubscriber.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace Kinova {

auto ControlInterface::start() -> void {
	doStart();
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->onInterfaceStarted(); });
}

auto ControlInterface::stop() -> void {
	doStop();
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->onInterfaceStopped(); });
}

auto ControlInterface::subscribe(SubscriberPointer subscriber) -> bool {
	assert(subscriber);
	return subscribers.insert(subscriber).second;
}

auto ControlInterface::unsubscribe(SubscriberPointer subscriber) -> bool {
	assert(subscriber);
	return subscribers.erase(subscriber);
}

auto ControlInterface::notifySubscribers(Command command) -> void {
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->process(command); });
}

} // namespace Kinova