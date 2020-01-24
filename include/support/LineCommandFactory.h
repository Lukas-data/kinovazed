#ifndef INCLUDE_SUPPORT_LINE_COMMAND_FACTORY_H_
#define INCLUDE_SUPPORT_LINE_COMMAND_FACTORY_H_

#include "comm/Command.h"

#include <optional>
#include <string>

namespace KinovaZED {

auto lineCommandFactory(std::string data) -> std::optional<Comm::Command>;

}

#endif