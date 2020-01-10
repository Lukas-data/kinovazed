#ifndef INCLUDE_CONTROL_INTERFACE_H_
#define INCLUDE_CONTROL_INTERFACE_H_

#include "Command.h"

#include <memory>
#include <set>

namespace Kinova {

struct ControlInterface {
	using SubscriberPointer = std::shared_ptr<struct CommandSubscriber>;

	virtual ~ControlInterface() noexcept;

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
	auto virtual do_start() -> void = 0;

	/**
	 * Concrete implementation of interface stop
	 */
	auto virtual do_stop() -> void = 0;

	/**
	 * Notify all known subscribers about an incoming command
	 */
	auto notifySubscribers(Command command) -> void;

  private:
	std::set<SubscriberPointer> subscribers;
};

} // namespace Kinova

#endif