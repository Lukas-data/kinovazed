#include "comm/TCPInterface.h"

#include "comm/Notification.h"
#include "support/Logging.h"
#include "support/StringUtils.h"

#include <asio/error.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>
#include <spdlog/fmt/fmt.h>

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <exception>
#include <istream>
#include <iterator>
#include <limits>
#include <string>

namespace KinovaZED::Comm {

TCPInterface::TCPInterface(CommandFactory commandFactory,
                           asio::io_context &networkContext,
                           std::uint16_t port,
                           Logger logger)
    : CommandInterface{commandFactory}
    , LoggingMixin{logger, "TCPInterface"}
    , networkContext{networkContext}
    , acceptor{networkContext, asio::ip::tcp::endpoint{asio::ip::address_v4::any(), port}, true}
    , remoteSocket{networkContext} {
	assert(logger);
}


TCPInterface::~TCPInterface() noexcept {
	stop();
}

auto TCPInterface::doStart() -> void {
	startAccepting();
}

auto TCPInterface::doSend(Notification message) -> void {
	enqueueAndSend(toString(message));
}

auto TCPInterface::doSend(Heartbeat message) -> void {
	enqueueAndSend(toString(message));
}

auto TCPInterface::doStop() -> void {
	auto ignored = asio::error_code{};
	if (remoteSocket && remoteSocket->is_open()) {
		remoteSocket->cancel(ignored);
		remoteSocket->close(ignored);
		remoteSocket->shutdown(remoteSocket->shutdown_both, ignored);
	}
	acceptor.close(ignored);
}

auto TCPInterface::startAccepting() -> void {
	assert(remoteSocket);
	logDebug("doStart",
	         "starting to accept connections on '{}:{}'",
	         acceptor.local_endpoint().address().to_string(),
	         acceptor.local_endpoint().port());
	acceptor.async_accept(*remoteSocket, [&](auto error) { handleAccept(error); });
}

auto TCPInterface::handleAccept(asio::error_code error) -> void {
	if (error) {
		logError("handleAccept",
		         "an error occurred while trying to accept a connection! code: {0}, reason: {1}",
		         error.value(),
		         error.message());
	} else {
		logInfo("handleAccept",
		        "accepted a new connection from '{0}",
		        remoteSocket->remote_endpoint().address().to_string());
		startReading();
	}
}

auto TCPInterface::startReading() -> void {
	using buffer_iterator = std::istream_iterator<char>;

	async_read_until(*remoteSocket, readBuffer, '\n', [this](auto error, auto read) {
		if (error) {
			processReadError(error);
		} else if (read > 0) {
			logDebug("<startReading::lambda>", "read {0} bytes", read);
			auto messageStream = std::istream{&readBuffer};
			messageStream.unsetf(std::ios::skipws);
			copy_n(buffer_iterator{messageStream}, read - 1, back_inserter(messageBuffer));
			messageStream.ignore();
			trim(messageBuffer);
			processMessage(messageBuffer);
			messageBuffer.clear();
			startReading();
		}
	});
}

auto TCPInterface::processMessage(std::string message) -> void {
	logDebug("processMessage", "received message '{0}'", message);
	auto command = makeCommand(message);
	if (!command) {
		logError("processMessage", "failed to decode incomming command. raw data: '{0}'", message);
		return;
	}

	notifySubscribers(*command);
}


auto TCPInterface::enqueueAndSend(std::string message) -> void {
	if (!remoteSocket || !remoteSocket->is_open()) {
		logWarning("doSend", "no commander is connected. dropping outgoing message");
		return;
	}

	networkContext.post([this, message = std::move(message)] {
		sendBuffer.push(message + "\r\n");
		if (sendBuffer.size() == 1) {
			startSending();
		}
	});
}

auto TCPInterface::startSending() -> void {
	if (remoteSocket) {
		asio::async_write(*remoteSocket, asio::buffer(sendBuffer.front()), [this](auto error, auto written) {
			sendBuffer.pop();
			if (error) {
				logError("<startSending::lambda>",
				         "failed to send message to remote commander. code: {0}, reason: {1}",
				         error.value(),
				         error.message());
				return;
			}
			logDebug("<startSending::lambda>", "sent {} bytes to remote commander", written);
			if (!sendBuffer.empty()) {
				startSending();
			}
		});
	}
}

auto TCPInterface::processReadError(asio::error_code error) -> void {
	using buffer_iterator = std::istreambuf_iterator<char>;

	if (error == asio::error::eof || error == asio::error::connection_reset) {
		logInfo("processReadError", "the commander disconnected.");
		disconnectRemote();
		startAccepting();
	} else if (error == asio::error::not_found) {
		if (messageBuffer.size() < 1024) {
			logDebug("processReadError", "received a partial message. buffering...");
			auto messageStream = std::istream{&readBuffer};
			copy(buffer_iterator{messageStream}, buffer_iterator{}, back_inserter(messageBuffer));
			startReading();
		} else {
			logError("processReadError", "no message found within 1kB, disconnecting the commander");
			messageBuffer.clear();
			disconnectRemote();
			startAccepting();
		}
	} else {
		logError("processReadError",
		         "error while reading message from network. code: {0}, reason: {1}",
		         error.value(),
		         error.message());
	}
}

auto TCPInterface::disconnectRemote() -> void {
	try {
		remoteSocket->cancel();
		remoteSocket->shutdown(remoteSocket->shutdown_both);
		remoteSocket->close();
	} catch (std::exception const &e) {
		logError("disconnectRemote", "failed to cleanly disconnect the commander. reason: {0}", e.what());
	}

	remoteSocket.emplace(networkContext);
	auto bufferStream = std::istream(&readBuffer);
	bufferStream.ignore(std::numeric_limits<std::streamsize>::max());
}

} // namespace KinovaZED::Comm