#ifndef INCLUDE_COMMAND_H_
#define INCLUDE_COMMAND_H_

#include "ToString.h"

#include <cstdint>
#include <optional>

namespace Kinova {

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

template<>
auto toString(Command::Id const &) -> std::string;

auto isKnownCommandId(int candidate) -> bool;

} // namespace Kinova

#endif