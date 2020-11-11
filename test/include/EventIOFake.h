#ifndef EVENTIOFAKE_H_
#define EVENTIOFAKE_H_

#include "comm/TCPInterface.h"

#include <memory>

template<typename ReadGenerator>
struct EventIOFake {
	explicit EventIOFake(std::shared_ptr<ReadGenerator> packetGenerator)
	    : packetGenerator{packetGenerator} {
	}

	auto readPacket() -> RoboRioProtocol::Packet {
		return (*packetGenerator)();
	}

	auto sendTCP(RoboRioProtocol::Packet command) -> bool {
		return packetGenerator->check(command);
	}

	void connect() {
	}

	void disconnect() {
	}

  private:
	std::shared_ptr<ReadGenerator> packetGenerator;
};


#endif /* EVENTIOFAKE_H_ */
