#ifndef PROTOCOL_H_
#define PROTOCOL_H_

namespace RoboRioProtocol {


constexpr auto commandLength = 6;
constexpr auto dataLength = 6;
constexpr auto dataPackages = 4;
constexpr int errorCommand = 99;

struct Packet {
	int command;
	int var;
	int x;
	int y;
	int z;
};

//TODO: (tcorbat) Specify input and output operators


}


#endif /* PROTOCOL_H_ */
