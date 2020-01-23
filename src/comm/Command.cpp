#include "comm/Command.h"

#include "support/EnumUtils.h"
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
    std::pair{Command::Id::GoToPosition, "GoToPosition"},
    std::pair{Command::Id::Initialize, "Initialize"},
    std::pair{Command::Id::MoveJoystick, "MoveJoystick"},
    std::pair{Command::Id::NoMode, "NoMode"},
    std::pair{Command::Id::QuitEStop, "QuitEStop"},
    std::pair{Command::Id::SetMode, "SetMode"},
    std::pair{Command::Id::Unfold, "Unfold"},
};

static_assert(enumNameMappingsAreUnique(commandNames), "Duplicate entry in name map!");
static_assert(enumNameMapHasAllEntries(commandNames, Command::Id::Initialize), "Missing entry in name map!");

// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
auto trim(std::string &s) -> void {
	s.erase(begin(s), find_if(begin(s), end(s), [](int ch) { return !std::isspace(ch); }));
	s.erase(find_if(rbegin(s), rend(s), [](int ch) { return !std::isspace(ch); }).base(), end(s));
}

auto isKnownCommandId(int candidate) -> bool {
	return candidate >= 0 && candidate < static_cast<int>(Command::Id::END_OF_ENUM);
}

auto isKnownCommandId(std::string const &candidate) -> bool {
	auto found =
	    std::find_if(cbegin(commandNames), cend(commandNames), [&](auto entry) { return entry.second == candidate; });
	return found != cend(commandNames);
}

auto checkParameterCount(Command::Id id, std::vector<std::any> parameters) -> bool {
	switch (id) {
	case Command::Id::EStop:
	case Command::Id::QuitEStop:
	case Command::Id::Initialize:
	case Command::Id::Unfold:
	case Command::Id::NoMode:
		if (!parameters.empty()) {
			return false;
		}
		break;
	case Command::Id::GoToPosition:
	case Command::Id::SetMode:
		if (parameters.size() != 1) {
			return false;
		}
		break;
	case Command::Id::MoveJoystick:
		if (parameters.size() != 3) {
			return false;
		}
		break;
	default:
		return false;
	}

	return true;
}

auto adjustParameterTypes(Command::Id id, std::vector<std::any> &parameters) -> bool {
	switch (id) {
	case Command::Id::EStop:
	case Command::Id::QuitEStop:
	case Command::Id::Initialize:
	case Command::Id::Unfold:
	case Command::Id::NoMode:
	case Command::Id::GoToPosition:
	case Command::Id::SetMode:
		return true;
	case Command::Id::MoveJoystick:
		try {
			transform(cbegin(parameters), cend(parameters), begin(parameters), [](auto raw) {
				return stoi(std::any_cast<std::string>(raw));
			});
			return true;
		} catch (std::exception const &) {
			return false;
		}
		break;
	default:
		return false;
	}
}

auto parseCommand(std::string const &data) -> std::optional<Command> {
	auto dataStream = std::istringstream{data};

	auto name = std::string{};
	std::getline(dataStream, name, ':');
	trim(name);
	if (name.empty() || !isKnownCommandId(name)) {
		return std::nullopt;
	}

	auto id = fromString<Command::Id>(name);

	auto parameters = std::vector<std::any>{};
	auto parameter = std::string{};
	while (std::getline(dataStream, parameter, ':')) {
		parameters.push_back(parameter);
	}

	if (!checkParameterCount(id, parameters)) {
		return std::nullopt;
	}

	if (!adjustParameterTypes(id, parameters)) {
		return std::nullopt;
	}

	return Command{id, parameters};
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