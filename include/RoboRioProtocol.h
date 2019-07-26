#ifndef PROTOCOL_H_
#define PROTOCOL_H_

namespace RoboRioProtocol {


constexpr auto commandLength = 6;
constexpr auto dataLength = 6;
constexpr auto dataPackages = 4;
constexpr int errorCommand = 99;

struct Packet {
	int command = 0;
	int var = 0;
	int x = 0;
	int y = 0;
	int z = 0;
};

//TODO: (tcorbat) Specify input and output operators


}


#endif /* PROTOCOL_H_ */
