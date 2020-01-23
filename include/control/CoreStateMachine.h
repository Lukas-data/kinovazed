#ifndef INCLUDE_CONTROL_CORE_STATE_MACHINE_H_
#define INCLUDE_CONTROL_CORE_STATE_MACHINE_H_

#include "hw/Actor.h"
#include "support/Logging.h"

#include <sml/sml.hpp>

namespace KinovaZED::Control {

struct CoreStateMachine : LoggingMixin {

	struct Event {
		template<typename Impl>
		struct ActorEventBase {
			auto operator()() const -> void {
				static_cast<Impl &> (*this)();
			}

			Hw::Actor &actor;
		};

		struct Initialize : ActorEventBase<Initialize> {
			auto operator()() const -> void;
		};

		struct Initialized {};

		struct EStop : ActorEventBase<EStop> {
			auto operator()() const -> void;
		};

		struct QuitEStop : ActorEventBase<QuitEStop> {
			auto operator()() const -> void;
		};

		struct Retract : ActorEventBase<Retract> {
			auto operator()() const -> void;
		};

		struct Retracted {};

		struct Unfold : ActorEventBase<Unfold> {
			auto operator()() const -> void;
		};

		struct Unfolded {};

		struct SetMode : ActorEventBase<SetMode> {
			auto operator()() const -> void;

			Hw::SteeringMode mode;
		};

		struct GoToPosition : ActorEventBase<GoToPosition> {
			auto operator()() const -> void;

			Hw::Coordinates position;
		};

		struct ModeSet {
			Hw::SteeringMode mode;
		};

		struct JoystickMoved : ActorEventBase<JoystickMoved> {
			auto operator()() const -> void;

			int x;
			int y;
			int z;
		};

		struct SequenceFinished {};
	};

	explicit CoreStateMachine(Logger logger)
	    : LoggingMixin{logger, "CoreStateMachine"} {
	}

	auto operator()() noexcept {
		using namespace boost::sml;
		using boost::sml::on_exit;

		auto isSteeringMode = [](auto const &e) { return e.mode != Hw::SteeringMode::NoMode; };

		auto eventAction = [](auto const &e) { e(); };

		auto logEntry = [this](auto state) {
			return [this, state] { logDebug("<transition>", "entered state '{}'", state); };
		};

		auto logExit = [this](auto state) {
			return [this, state] { logDebug("<transition>", "left state '{}'", state); };
		};

		// clang-format off
		return make_transition_table(
		    // [poweredOff]
		    *"poweredOff"_s + event<Event::Initialize> / eventAction = "initializing"_s,
		    "poweredOff"_s  + event<Event::EStop>      / eventAction = "emergencyStopped"_s,
			"poweredOff"_s  + on_entry<_>              / logEntry("poweredOff"),
			"poweredOff"_s  + on_exit<_>               / logExit("poweredOff"),
			
		    // [poweredOff]
		    "initializing"_s + event<Event::Initialized>               = "initialized"_s,
		    "initializing"_s  + event<Event::EStop>      / eventAction = "emergencyStopped"_s,
			"initializing"_s  + on_entry<_>              / logEntry("initializing"),
			"initializing"_s  + on_exit<_>               / logExit("initializing"),

			// [initialized]
			"initialized"_s + event<Event::Retract> / eventAction = "retracting"_s,
		    "initialized"_s + event<Event::EStop>   / eventAction = "emergencyStopped"_s,
			"initialized"_s  + on_entry<_>          / logEntry("initialized"),
			"initialized"_s  + on_exit<_>           / logExit("initialized"),

			// [retracting]
			"retracting"_s + event<Event::Retracted>               = "retracted"_s,
		    "retracting"_s + event<Event::EStop>     / eventAction = "emergencyStopped"_s,
			"retracting"_s  + on_entry<_>            / logEntry("retracting"),
			"retracting"_s  + on_exit<_>             / logExit("retracting"),

			// [retracted]
			"retracted"_s + event<Event::Unfold> / eventAction = "unfolding"_s,
		    "retracted"_s + event<Event::EStop>  / eventAction = "emergencyStopped"_s,
			"retracted"_s  + on_entry<_>         / logEntry("retracted"),
			"retracted"_s  + on_exit<_>          / logExit("retracted"),

			// [unfolded]
			"unfolding"_s + event<Event::Unfolded>               = "idle"_s,
		    "unfolding"_s + event<Event::EStop>    / eventAction = "emergencyStopped"_s,
			"unfolding"_s  + on_entry<_>           / logEntry("unfolding"),
			"unfolding"_s  + on_exit<_>            / logExit("unfolding"),

			// [idle]
			"idle"_s + event<Event::SetMode>      / eventAction = "settingMode"_s,
			"idle"_s + event<Event::GoToPosition> / eventAction = "runningSequence"_s,
			"idle"_s + event<Event::Retract>      / eventAction = "retracting"_s,
			"idle"_s + event<Event::Unfold>       / eventAction = "unfolding"_s,
		    "idle"_s + event<Event::EStop>        / eventAction = "emergencyStopped"_s,
			"idle"_s  + on_entry<_>               / logEntry("idle"),
			"idle"_s  + on_exit<_>                / logExit("idle"),

			// [settingMode]
			"settingMode"_s + event<Event::ModeSet> [ isSteeringMode ]                = "steering"_s,
			"settingMode"_s + event<Event::ModeSet> [ !isSteeringMode ]               = "idle"_s,
			"settingMode"_s + event<Event::Retract>                     / eventAction = "retracting"_s,
		    "settingMode"_s + event<Event::EStop>                       / eventAction = "emergencyStopped"_s,
			"settingMode"_s  + on_entry<_>                              / logEntry("settingMode"),
			"settingMode"_s  + on_exit<_>                               / logExit("settingMode"),

			// [steering]
			"steering"_s + event<Event::SetMode>       / eventAction = "settingMode"_s,
			"steering"_s + event<Event::JoystickMoved> / eventAction = "steering"_s,
			"steering"_s + event<Event::GoToPosition>  / eventAction = "runningSequence"_s,
			"steering"_s + event<Event::Retract>       / eventAction = "retracting"_s,
		    "steering"_s + event<Event::EStop>         / eventAction = "emergencyStopped"_s,
			"steering"_s  + on_entry<_>                / logEntry("steering"),
			"steering"_s  + on_exit<_>                 / logExit("steering"),

			// [runningSequence]
			"runningSequence"_s + event<Event::GoToPosition>     / eventAction = "runningSequence"_s,
			"runningSequence"_s + event<Event::SequenceFinished>               = "idle"_s,
		    "runningSequence"_s + event<Event::EStop>            / eventAction = "emergencyStopped"_s,
			"runningSequence"_s  + on_entry<_>                   / logEntry("runningSequence"),
			"runningSequence"_s  + on_exit<_>                    / logExit("runningSequence"),
			
		    // [emergencyStopped]
		    "emergencyStopped"_s + event<Event::QuitEStop> = "poweredOff"_s,
			"emergencyStopped"_s  + on_entry<_>            / logEntry("emergencyStopped"),
			"emergencyStopped"_s  + on_exit<_>             / logExit("emergencyStopped")
		    // clang-format on
		);
	}
};

} // namespace KinovaZED::Control

#endif