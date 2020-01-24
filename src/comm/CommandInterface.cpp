#include "comm/CommandInterface.h"

#include "comm/Command.h"
#include "comm/CommandSubscriber.h"
#include "comm/Heartbeat.h"
#include "comm/Notification.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace KinovaZED::Comm {

CommandInterface::CommandInterface(CommandFactory commandFactory)
    : commandFactory{commandFactory} {
}

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

auto CommandInterface::send(Notification message) -> void {
	doSend(message);
}

auto CommandInterface::send(Heartbeat message) -> void {
	doSend(message);
}

auto CommandInterface::notifySubscribers(Command command) -> void {
	std::for_each(begin(subscribers), end(subscribers), [&](auto subscriber) { subscriber->process(command); });
}

auto CommandInterface::makeCommand(std::string data) -> std::invoke_result_t<CommandFactory, std::string> {
	assert(commandFactory);
	return commandFactory(data);
}

} // namespace KinovaZED::Comm