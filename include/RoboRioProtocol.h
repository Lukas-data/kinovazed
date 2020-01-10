#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "Event.h"

#include <ostream>
#include <string>
#include <tuple>

namespace RoboRioProtocol {


constexpr auto commandLength = 6;
constexpr auto dataLength = 6;
constexpr auto dataPackages = 4;
auto constexpr messageLength = commandLength + (dataPackages * dataLength);

constexpr int errorCommand = 99; // TODO (tcorbat): Currently this error is coerced into a Packet (command). This is not
                                 // nice, as 99 is not a proper value for KinovaFSM::Event

struct Packet {
	KinovaFSM::Event command = KinovaFSM::NoEvent;
	int var = 0;
	int x = 0;
	int y = 0;
	int z = 0;
};

inline auto operator==(Packet const &lhs, Packet const &rhs) {
	return std::tie(lhs.command, lhs.var, lhs.x, lhs.y, lhs.z) == std::tie(rhs.command, rhs.var, rhs.x, rhs.y, rhs.z);
}

inline auto operator!=(Packet const &lhs, Packet const &rhs) {
	return !(lhs == rhs);
}

inline auto operator<<(std::ostream &out, Packet const &packet) -> std::ostream & {
	using namespace std::string_literals;
	int commandNumber = packet.command;
	auto commandName =
	    commandNumber > 38 ? "<unknown> #"s + std::to_string(commandNumber) : KinovaFSM::eventNames[commandNumber];
	out << "RoboRIO Protocol Packet[" << commandName << ", " << packet.var << ", x=" << packet.x << ", y=" << packet.y
	    << ", z=" << packet.z << ']';
	return out;
}

// TODO: (tcorbat) Specify input and output operators


} // namespace RoboRioProtocol


#endif /* PROTOCOL_H_ */
