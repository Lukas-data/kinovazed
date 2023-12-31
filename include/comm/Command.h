#ifndef INCLUDE_COMM_COMMAND_H_
#define INCLUDE_COMM_COMMAND_H_

#include "support/ToString.h"

#include <any>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace KinovaZED::Comm {

struct Command {
	enum struct Id : std::uint8_t {
		Initialize,
		Unfold,
		Retract,
		Freeze,
		Unfreeze,
		SetJoystickMode,
		MoveJoystick,
		RunObjective,
		EStop,
		QuitEStop,
		SetActiveObjective,
		GetObjectivePosition,
		GetAbsolutePosition,

		// End Marker
		END_OF_ENUM
	};

	Id id;
	std::vector<std::any> parameters;
};

auto isKnownCommandId(int candidate) -> bool;

auto isKnownCommandId(std::string const &candidate) -> bool;

} // namespace KinovaZED::Comm

namespace KinovaZED {

template<>
auto toString(Comm::Command::Id const &) -> std::string;

template<>
auto fromString(std::string const &) -> Comm::Command::Id;

} // namespace KinovaZED

#endif