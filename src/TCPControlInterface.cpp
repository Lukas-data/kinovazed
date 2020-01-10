#include "TCPControlInterface.h"

#include "Logging.h"

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>

#include <cassert>
#include <cstdint>
#include <sstream>
#include <string>

namespace Kinova {

TCPControlInterface::TCPControlInterface(asio::io_context &networkContext, std::uint16_t port, Logging::Logger logger)
    : logger{logger}
    , networkContext{networkContext}
    , acceptor{networkContext, asio::ip::tcp::v4(), port} {
	assert(logger);
}

auto TCPControlInterface::doStart() -> void {
	acceptor.async_accept(remoteSocket, [&](auto error) { handle_accept(error); });
}

auto TCPControlInterface::doStop() -> void {
	if (remoteSocket.is_open()) {
		remoteSocket.close();
	}
	acceptor.close();
}

auto TCPControlInterface::handle_accept(asio::error_code error) -> void {
	if (error) {
		logger->error(
		    "TCPControlInterface: An error occurred while trying to accept a connection! code: {0} || reason: {1}",
		    error.value(),
		    error.message());
	} else {
		startReading();
	}
}

auto TCPControlInterface::startReading() -> void {
	async_read(remoteSocket, asio::buffer(readBuffer), [this](auto error, auto read) {
		if (error) {
			processReadError(error);
		} else {
			auto message = std::string{readBuffer.data(), read};
			processMessage(message);
			startReading();
		}
	});
}

auto TCPControlInterface::processMessage(std::string message) -> void {
	int id, variable, x, y, z;
	auto messageStream = std::istringstream{message};
	if (messageStream >> id >> variable >> x >> y >> z) {
		if (!isKnownCommandId(id)) {
			logger->warn("TCPControlInterface: ignoring unknown command. id: {0}", id);
		} else {
			auto command = Command{static_cast<Command::Id>(id), variable, x, y, z};
		}
	} else {
		logger->error("TCPControlInterface: failed to decode incomming command.");
	}
}

auto TCPControlInterface::processReadError(asio::error_code error) -> void {
	logger->error("TCPControlInterface: error while reading message from network. code: {0} || reason: {1}",
	              error.value(),
	              error.message());
}

} // namespace Kinova