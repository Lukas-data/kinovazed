#ifndef INCLUDE_TCP_CONTROL_INTERFACE_H_
#define INCLUDE_TCP_CONTROL_INTERFACE_H_

#include "comm/CommandInterface.h"
#include "support/Logging.h"

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/streambuf.hpp>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

namespace KinovaZED::Comm {

/// The length of the command field
constexpr auto commandLength = 6;

/// The length of one data field
constexpr auto dataLength = 6;

/// The number of data fields
constexpr auto dataPackages = 4;

/// The length of full TCP message
auto constexpr messageLength = commandLength + (dataPackages * dataLength);

struct TCPInterface : CommandInterface, LoggingMixin {
	TCPInterface(CommandFactory commandFactory, asio::io_context &networkContext, std::uint16_t port, Logger logger);

	~TCPInterface() noexcept;

  private:
	auto doStart() -> void;

	auto doStop() -> void;

	auto startAccepting() -> void;
	auto handleAccept(asio::error_code error) -> void;

	auto startReading() -> void;
	auto processMessage(std::string message) -> void;

	auto processReadError(asio::error_code error) -> void;

	auto disconnectRemote() -> void;

	asio::io_context &networkContext;
	asio::ip::tcp::acceptor acceptor;
	std::optional<asio::ip::tcp::socket> remoteSocket;

	asio::streambuf readBuffer{128};
	std::string messageBuffer{};
};

} // namespace KinovaZED::Comm

#endif