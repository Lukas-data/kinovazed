#ifndef INCLUDE_COMM_COMMAND_H_
#define INCLUDE_COMM_COMMAND_H_

#include "support/ToString.h"

#include <cstdint>
#include <optional>

namespace KinovaZED::Comm {

struct Command {

	enum struct Id : std::uint8_t {
		NoOp,
		Tick,
		Initialize,
		NoMode,
		Retract,
		Unfold,
		SetMode,
		MoveJoystick,
		GoToPosition,
		Teach,
		SavePoint,
		SaveOrigin,
		DeletePoint,
		Previous,
		Next,
		MoveOrigin,
		Exit,
		Shutdown,
		E_Stop,
		QuitEStop,

		// End Marker
		END_OF_ENUM
	};

	auto constexpr operator==(Command const &other) const -> bool {
		return id == other.id && variable == other.variable;
	}

	auto constexpr operator!=(Command const &other) const -> bool {
		return !(*this == other);
	}

	Id id;
	int variable{};
	int x{};
	int y{};
	int z{};
};

auto isKnownCommandId(int candidate) -> bool;

} // namespace KinovaZED::Comm

namespace KinovaZED {

template<>
auto toString(Comm::Command::Id const &) -> std::string;

}

#endif