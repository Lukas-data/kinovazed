#include "TCPServer.h"

#include "spdlog/spdlog.h"

#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <array>
#include <sstream>
#include <stdexcept>

constexpr auto rioDummy = true;
constexpr auto bufferSize = 256;
constexpr auto messageLength =
    RoboRioProtocol::commandLength + (RoboRioProtocol::dataPackages * RoboRioProtocol::dataLength);
constexpr auto numberOfQueueConnections = 5;

void TCPServer::error(const char *funcName, const char *msg) {
	logger->error("TCPServer::{0}(): {1}", funcName, msg);
}

TCPServer::TCPServer(Logging::Logger logger)
    : logger{logger}
    , serverSocket{socket(AF_INET, SOCK_STREAM, 0)} {
	if (serverSocket < 0) {
		throw std::runtime_error{"Error opening socket"};
	}

	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		throw std::runtime_error{"Error on setting socket options"};
	}

	sockaddr_in serv_addr{};
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		throw std::runtime_error{"Error on binding socket"};
	}

	if (listen(serverSocket, numberOfQueueConnections) == -1) {
		throw std::runtime_error{"Error on listening on socket"};
	}
}

TCPServer::~TCPServer() noexcept {
	if (serverSocket > 0) {
		close(serverSocket);
	}
}

void TCPServer::connect() {
	if (!rioDummy) {
		if (clientSocket > 0) {
			disconnect();
		}
		int clilen = sizeof(cli_addr);
		if ((clientSocket = accept(serverSocket, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen)) < 0) {
			throw std::runtime_error{"Error accepting client connection"};
		}
	}
}

void TCPServer::disconnect() {
	if (clientSocket) {
		close(clientSocket);
		clientSocket = 0;
	}
}

auto TCPServer::sendTCP(RoboRioProtocol::Packet packet) -> bool {
	if (!rioDummy) {
		logger->debug("TCPServer::sendTCP(): start");
		std::array<char, messageLength + 1> buffer{};
		int m = snprintf(
		    buffer.data(), buffer.size(), "%6d%6d%6d%6d%6d", packet.command, packet.var, packet.x, packet.y, packet.z);
		if (m != messageLength) {
			error("sendTCP", "Error preparing message");
			return false;
		}
		logger->debug("TCPServer::sendTCP(): Message prepared.");
		if (write(clientSocket, buffer.data(), messageLength) < 0) {
			error("sendTCP", "Error writing to socket");
			return false;
		}
	}
	logger->debug(
	    "TCPServer::sendTCP({0}, {1}, {2}, {3}, {4})", packet.command, packet.var, packet.x, packet.y, packet.z);
	return true;
}

auto TCPServer::readPacket() -> RoboRioProtocol::Packet {
	std::array<char, bufferSize> buffer{};
	int totalRead = 0;
	for (int numberOfTries = 1; numberOfTries < retryLimit; numberOfTries++) {
		int n = read(clientSocket, buffer.data() + totalRead, messageLength - totalRead);
		totalRead += n;
		if (totalRead == messageLength) {
			std::istringstream input{buffer.data()};
			int command, var, x, y, z;
			if (input >> command >> var >> x >> y >> z) {
				KinovaFSM::Event event = static_cast<KinovaFSM::Event>(command);
				return RoboRioProtocol::Packet{event, var, x, y, z};
			}
			break;
		}
	}
	error("readTCP", "Connection Lost");
	KinovaFSM::Event errorEvent = static_cast<KinovaFSM::Event>(RoboRioProtocol::errorCommand);
	return RoboRioProtocol::Packet{errorEvent};
}
