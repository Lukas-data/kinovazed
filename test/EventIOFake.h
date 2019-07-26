#ifndef EVENTIOFAKE_H_
#define EVENTIOFAKE_H_

#include "RoboRioProtocol.h"

#include <utility>

template <typename ReadGenerator>
struct EventIOFake {
	auto readPacket() -> RoboRioProtocol::Packet {
		return packetGenerator();
	}

	auto sendTCP(RoboRioProtocol::Packet command) -> bool {
		return true;
	}

	void connect() {

	}

	void disconnect() {

	}

private:
	ReadGenerator packetGenerator{};
};


#endif /* EVENTIOFAKE_H_ */
