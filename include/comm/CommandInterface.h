#ifndef INCLUDE_COMM_COMMAND_INTERFACE_H_
#define INCLUDE_COMM_COMMAND_INTERFACE_H_

#include "comm/Command.h"
#include "comm/Heartbeat.h"
#include "comm/Notification.h"

#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <type_traits>

namespace KinovaZED::Comm {

struct CommandInterface {
	using SubscriberPointer = std::shared_ptr<struct CommandSubscriber>;
	using CommandFactory = std::function<std::optional<Command>(std::string)>;

	explicit CommandInterface(CommandFactory commandFactory);

	virtual ~CommandInterface() noexcept = default;

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

	/**
	 * Send a notification to the remote controller
	 */
	auto send(Notification message) -> void;

	/**
	 * Send a heartbeat to the remote controller
	 */
	auto send(Heartbeat message) -> void;

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
	 * Concrete implementation of interface send
	 */
	auto virtual doSend(Notification message) -> void = 0;

	/**
	 * Concrete implementation of interface send
	 */
	auto virtual doSend(Heartbeat message) -> void = 0;

	/**
	 * Notify all known subscribers about an incoming command
	 */
	auto notifySubscribers(Command command) -> void;

	/**
	 * Create a command by processing the provided data using the command factory
	 */
	auto makeCommand(std::string data) -> std::invoke_result_t<CommandFactory, std::string>;

  private:
	CommandFactory commandFactory;
	std::set<SubscriberPointer> subscribers;
};

} // namespace KinovaZED::Comm

#endif