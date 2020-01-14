#include "comm/CommandInterface.h"

#include "comm/Command.h"
#include "comm/CommandSubscriber.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace KinovaZED::Comm {

auto CommandInterface::start() -> void {
	doStart();
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->onInterfaceStarted(); });
}

auto CommandInterface::stop() -> void {
	doStop();
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->onInterfaceStopped(); });
}

auto CommandInterface::subscribe(SubscriberPointer subscriber) -> bool {
	assert(subscriber);
	return subscribers.insert(subscriber).second;
}

auto CommandInterface::unsubscribe(SubscriberPointer subscriber) -> bool {
	assert(subscriber);
	return subscribers.erase(subscriber);
}

auto CommandInterface::notifySubscribers(Command command) -> void {
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->process(command); });
}

} // namespace KinovaZED::Comm