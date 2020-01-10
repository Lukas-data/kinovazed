#ifndef INCLUDE_TCP_CONTROL_INTERFACE_H_
#define INCLUDE_TCP_CONTROL_INTERFACE_H_

#include "ControlInterface.h"
#include "Logging.h"
#include "RoboRioProtocol.h"

#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include <array>
#include <cstddef>
#include <cstdint>

namespace Kinova {

struct TCPControlInterface : private ControlInterface {
	TCPControlInterface(asio::io_context &networkContext, std::uint16_t port, Logging::Logger logger);

  private:
	auto doStart() -> void;

	auto doStop() -> void;

	auto handle_accept(asio::error_code error) -> void;

	auto startReading() -> void;

	auto processMessage(std::string message) -> void;

	auto processReadError(asio::error_code error) -> void;

	Logging::Logger logger;

	asio::io_context &networkContext;
	asio::ip::tcp::acceptor acceptor;
	asio::ip::tcp::socket remoteSocket{networkContext};

	std::array<char, RoboRioProtocol::messageLength + 1> readBuffer{};
};

} // namespace Kinova

#endif