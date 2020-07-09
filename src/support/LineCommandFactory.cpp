#include "support/LineCommandFactory.h"

#include "comm/Command.h"
#include "control/Objective.h"
#include "hw/Actor.h"
#include "support/StringUtils.h"

#include <algorithm>
#include <any>
#include <exception>
#include <iterator>
#include <string>
#include <vector>

namespace KinovaZED {

auto checkParameterCount(Comm::Command::Id id, std::vector<std::any> parameters) -> bool {
	using namespace Comm;

	switch (id) {
	case Command::Id::EStop:
	case Command::Id::QuitEStop:
	case Command::Id::Initialize:
	case Command::Id::Unfold:
	case Command::Id::Retract:
	case Command::Id::Freeze:
	case Command::Id::Unfreeze:
	case Command::Id::GetCurrentPosition:
		if (!parameters.empty()) {
			return false;
		}
		break;
	case Command::Id::RunObjective:
	case Command::Id::SetJoystickMode:
		if (parameters.size() != 1) {
			return false;
		}
		break;
	case Command::Id::MoveJoystick:
		if (parameters.size() != 3) {
			return false;
		}
		break;
	case Command::Id::END_OF_ENUM:
		return false;
	}

	return true;
}

auto adjustParameterTypes(Comm::Command::Id id, std::vector<std::any> &parameters) -> bool {
	using namespace Comm;

	switch (id) {
	case Command::Id::EStop:
	case Command::Id::QuitEStop:
	case Command::Id::Initialize:
	case Command::Id::Unfold:
	case Command::Id::Retract:
	case Command::Id::Freeze:
	case Command::Id::Unfreeze:
	case Command::Id::GetCurrentPosition:
		return true;
	case Command::Id::RunObjective:
		try {
			auto objectiveName = std::any_cast<std::string>(parameters.at(0));
			if (!Control::isKnownObjectiveId(objectiveName)) {
				return false;
			}
			parameters.front() = fromString<Control::Objective::Id>(objectiveName);
			return true;
		} catch (std::exception const &) {
			return false;
		}
	case Command::Id::SetJoystickMode:
		try {
			auto modeName = std::any_cast<std::string>(parameters.at(0));
			if (!Hw::isKnownSteeringMode(modeName)) {
				return false;
			}
			parameters.front() = fromString<Hw::SteeringMode>(modeName);
			return true;
		} catch (std::exception const &) {
			return false;
		}
	case Command::Id::MoveJoystick:
		try {
			transform(cbegin(parameters), cend(parameters), begin(parameters), [](auto raw) {
				return stoi(std::any_cast<std::string>(raw));
			});
			return true;
		} catch (std::exception const &) {
			return false;
		}
	case Command::Id::END_OF_ENUM:
		return false;
	}

	return false;
}

auto lineCommandFactory(std::string data) -> std::optional<Comm::Command> {
	using namespace Comm;

	auto dataStream = std::istringstream{data};

	auto name = std::string{};
	std::getline(dataStream, name, ':');
	trim(name);
	if (name.empty() || !Comm::isKnownCommandId(name)) {
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

} // namespace KinovaZED