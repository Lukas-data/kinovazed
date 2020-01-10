#include "Command.h"

#include "EnumUtils.h"
#include "ToString.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace Kinova {

auto constexpr commandNames = std::array{
    std::pair{Command::Id::DeletePoint, "DeletePoint"},
    std::pair{Command::Id::E_Stop, "E_Stop"},
    std::pair{Command::Id::Exit, "Exit"},
    std::pair{Command::Id::GoToPosition, "GoToPosition"},
    std::pair{Command::Id::Initialize, "Initialize"},
    std::pair{Command::Id::MoveJoystick, "MoveJoystick"},
    std::pair{Command::Id::MoveOrigin, "MoveOrigin"},
    std::pair{Command::Id::Next, "Next"},
    std::pair{Command::Id::NoMode, "NoMode"},
    std::pair{Command::Id::NoOp, "NoOp"},
    std::pair{Command::Id::Previous, "Previous"},
    std::pair{Command::Id::QuitEStop, "QuitEStop"},
    std::pair{Command::Id::Retract, "Retract"},
    std::pair{Command::Id::SaveOrigin, "SaveOrigin"},
    std::pair{Command::Id::SavePoint, "SavePoint"},
    std::pair{Command::Id::SetMode, "SetMode"},
    std::pair{Command::Id::Shutdown, "Shutdown"},
    std::pair{Command::Id::Teach, "Teach"},
    std::pair{Command::Id::Tick, "Tick"},
    std::pair{Command::Id::Unfold, "Unfold"},
};

static_assert(enumNameMappingsAreUnique(commandNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(commandNames, Command::Id::NoOp), "Missing entry in name map!");

template<>
auto toString(Command::Id const &id) -> std::string {
	assert(isKnownCommandId(static_cast<std::underlying_type_t<Command::Id>>(id)));

	auto found = std::find_if(cbegin(commandNames), cend(commandNames), [&](auto entry) { return entry.first == id; });
	return found->second;
}

auto isKnownCommandId(int candidate) -> bool {
	return candidate >= 0 && candidate < static_cast<int>(Command::Id::END_OF_ENUM);
}

} // namespace Kinova