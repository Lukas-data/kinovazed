#ifndef INCLUDE_COMM_HEARTBEAT_H_
#define INCLUDE_COMM_HEARTBEAT_H_

#include "support/ToString.h"

#include <bitset>
#include <string>


namespace KinovaZED::Comm {

struct Heartbeat {
	std::bitset<8> state;
};

} // namespace KinovaZED::Comm

namespace KinovaZED {

template<>
auto toString(Comm::Heartbeat const &heartbeat) -> std::string;

}

#endif