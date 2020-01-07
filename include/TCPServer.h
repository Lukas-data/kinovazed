#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "Logging.h"
#include "RoboRioProtocol.h"

#include <netinet/in.h>


constexpr auto noConnectionCount = 20;

struct TCPServer {
	TCPServer(Logging::Logger logger);
	~TCPServer() noexcept;

	void connect();
	void disconnect();

	auto readPacket() -> RoboRioProtocol::Packet;
	auto sendTCP(RoboRioProtocol::Packet command) -> bool;

  private:
	Logging::Logger logger;
	bool connected{false};
	void error(const char *funcName, const char *msg);

	int serverSocket{};
	int clientSocket{};
	int portno{51717};
	sockaddr_in cli_addr{};

	int retryLimit{noConnectionCount};
};
#endif
