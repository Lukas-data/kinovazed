#ifndef INCLUDE_COMM_COMMAND_INTERFACE_H_
#define INCLUDE_COMM_COMMAND_INTERFACE_H_

#include "comm/Command.h"

#include <memory>
#include <set>

namespace KinovaZED::Comm {

struct CommandInterface {
	using SubscriberPointer = std::shared_ptr<struct CommandSubscriber>;

	virtual ~CommandInterface() noexcept;

	/**
	 * Start receiving commands and sending responses/events
	 */
	auto start() -> void;

	/**
	 * Stop receiving commands and sending responses/events
	 */
	auto stop() -> void;

	/**
	 * Add a new subscriber to the list of known subscribers
	 *
	 * @return Whether or not a new subscription was added
	 */
	auto subscribe(SubscriberPointer subscriber) -> bool;

	/**
	 * Remove a subscriber from the list of known subscribers
	 *
	 * @return Whether or not the subscriber was previously subscribed
	 */
	auto unsubscribe(SubscriberPointer subscriber) -> bool;

  protected:
	/**
	 * Concrete implementation of interface start
	 */
	auto virtual doStart() -> void = 0;

	/**
	 * Concrete implementation of interface stop
	 */
	auto virtual doStop() -> void = 0;

	/**
	 * Notify all known subscribers about an incoming command
	 */
	auto notifySubscribers(Command command) -> void;

  private:
	std::set<SubscriberPointer> subscribers;
};

} // namespace KinovaZED::Comm

#endif