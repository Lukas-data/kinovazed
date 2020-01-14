#ifndef INCLUDE_TCP_CONTROL_INTERFACE_H_
#define INCLUDE_TCP_CONTROL_INTERFACE_H_

#include "comm/CommandInterface.h"
#include "support/Logging.h"

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace KinovaZED::Comm {

/// The length of the command field
constexpr auto commandLength = 6;

/// The length of one data field
constexpr auto dataLength = 6;

/// The number of data fields
constexpr auto dataPackages = 4;

/// The length of full TCP message
auto constexpr messageLength = commandLength + (dataPackages * dataLength);

struct TCPInterface : private CommandInterface {
	TCPInterface(asio::io_context &networkContext, std::uint16_t port, Logger logger);

  private:
	auto doStart() -> void;

	auto doStop() -> void;

	auto handle_accept(asio::error_code error) -> void;

	auto startReading() -> void;

	auto processMessage(std::string message) -> void;

	auto processReadError(asio::error_code error) -> void;

	Logger logger;

	asio::io_context &networkContext;
	asio::ip::tcp::acceptor acceptor;
	asio::ip::tcp::socket remoteSocket{networkContext};

	std::array<char, messageLength + 1> readBuffer{};
};

} // namespace KinovaZED::Comm

#endif