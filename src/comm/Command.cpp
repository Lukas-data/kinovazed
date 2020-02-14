#include "comm/Command.h"

#include "support/EnumUtils.h"
#include "support/StringUtils.h"
#include "support/ToString.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <iterator>
#include <locale>
#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace KinovaZED::Comm {

auto constexpr commandNames = std::array{
    std::pair{Command::Id::EStop, "EStop"},
    std::pair{Command::Id::RunObjective, "RunObjective"},
    std::pair{Command::Id::Initialize, "Initialize"},
    std::pair{Command::Id::MoveJoystick, "MoveJoystick"},
    std::pair{Command::Id::QuitEStop, "QuitEStop"},
    std::pair{Command::Id::SetMode, "SetMode"},
    std::pair{Command::Id::Unfold, "Unfold"},
    std::pair{Command::Id::Retract, "Retract"},
};

static_assert(enumNameMappingsAreUnique(commandNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(commandNames, Command::Id::Initialize), "Missing entry in name map!");

auto isKnownCommandId(int candidate) -> bool {
	return candidate >= 0 && candidate < static_cast<int>(Command::Id::END_OF_ENUM);
}

auto isKnownCommandId(std::string const &candidate) -> bool {
	auto found =
	    std::find_if(cbegin(commandNames), cend(commandNames), [&](auto entry) { return entry.second == candidate; });
	return found != cend(commandNames);
}

} // namespace KinovaZED::Comm

namespace KinovaZED {

using namespace Comm;

template<>
auto toString(Command::Id const &id) -> std::string {
	assert(isKnownCommandId(static_cast<std::underlying_type_t<Command::Id>>(id)));

	auto found = std::find_if(cbegin(commandNames), cend(commandNames), [&](auto entry) { return entry.first == id; });
	return found->second;
}

template<>
auto fromString(std::string const &name) -> Comm::Command::Id {
	auto found =
	    std::find_if(cbegin(commandNames), cend(commandNames), [&](auto entry) { return entry.second == name; });
	assert(found != cend(commandNames));
	return found->first;
}


} // namespace KinovaZED