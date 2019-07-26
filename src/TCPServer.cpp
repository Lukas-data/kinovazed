#include "TCPServer.h"
#include "Log.h"

#include <array>
#include <cstddef>

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sstream>
#include <stdexcept>

constexpr auto rioDummy = false;
constexpr auto bufferSize = 256;
constexpr auto messageLength = RoboRioProtocol::commandLength + (RoboRioProtocol::dataPackages * RoboRioProtocol::dataLength);
constexpr auto numberOfQueueConnections = 5;

void TCPServer::error(const char *funcName, const char *msg) {
	ALL_LOG(logERROR) << "TCPServer::" << funcName << "(): " << msg;
}

TCPServer::TCPServer() :
		serverSocket{socket(AF_INET, SOCK_STREAM, 0)} {
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
	if (bind(serverSocket, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
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

/*Opens TCP Server and establishes connection to a client(RoboRio)*/
void TCPServer::connect() {
	if (!rioDummy) {
		if (clientSocket > 0) {
			disconnect();
		}
		int clilen = sizeof(cli_addr);
		if ((clientSocket = accept(serverSocket, (struct sockaddr*) &cli_addr, (socklen_t*) &clilen)) < 0) {
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

/*sends Message to Client (RoboRio). Takes Command and data packages as input*/
auto TCPServer::sendTCP(RoboRioProtocol::Packet packet) -> bool {
	if (!rioDummy) {
		ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(): start";
		std::array<char, messageLength> buffer{};
		int m = sprintf(buffer.data(), "%6d%6d%6d%6d%6d", packet.command, packet.var, packet.x, packet.y, packet.z);
		if (m != messageLength) {
			error("sendTCP", "Error preparing message");
			return false;
		}
		ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(): Message prepared.";
		if (write(clientSocket, buffer.data(), messageLength) < 0) {
			error("sendTCP", "Error writing to socket");
			return false;
		}
	}
	ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(" << packet.command << ", " << packet.var << ", " << packet.x << ", " << packet.y << ", " << packet.z << ")";
	return true;
}


/*reads Message from Client (RoboRio). Saves Command to commandRecieved
 and Data to dataRecieved[]*/
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
				return RoboRioProtocol::Packet{command, var, x, y, z};
			}
			break;
		}
	}
	error("readTCP", "Connection Lost");
	return RoboRioProtocol::Packet{RoboRioProtocol::errorCommand}; //Report issue
}

	//read command and data from buffer

//	if (RIO_DUMMY == false) {
//		memset(buffer, '\0', bufferSize);
//
//		char commandString[commandLength];
//		char dataString[dataPackages][dataLength];
//
//		int n = read(newsockfd, buffer, messageLength);
//
//		if (n == 0 && retryLimit > 0) {
//			--retryLimit;
//			commandRecieved = 0;
//			return true;
//		} else if (n == 0 && retryLimit <= 0) {
//			error("readTCP", "Connection Lost");
//			retryLimit = noConnectionCount;
//			return false;
//		} else if (n != messageLength) {
//			error("readTCP", "Error reading from socket");
//			retryLimit = noConnectionCount;
//			return false;
//		}
//
//		memcpy(commandString, buffer, commandLength);
//		commandRecieved = atoi(buffer);
//		for (int i = 0; i < dataPackages; i++) {
//			memcpy(dataString[i], buffer + commandLength + dataLength * i, dataLength);
//			dataRecieved[i] = atoi(dataString[i]);
//		}
//		retryLimit = noConnectionCount;
//	}
//	return true;
//}


//int TCPServer::getData(int dataPackage) {
//	if (dataPackage < 0 || dataPackage > RoboRioProtocol::dataPackages - 1) {
//		ALL_LOG(logDEBUG4) << "no data[" << dataPackage << "] in communication protocol.";
//		return 0;
//	}
//	return dataRecieved[dataPackage];
//}

//int TCPServer::closeOnCommand(int command) {
//	close(clientSocket);
//	if (command == -2) {
//		return -1;
//	}
//	return 0;
//
//}

