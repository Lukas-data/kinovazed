#ifndef INCLUDE_CONTROL_CORE_STATE_MACHINE_H_
#define INCLUDE_CONTROL_CORE_STATE_MACHINE_H_

#include "hw/Actor.h"

#include <sml/sml.hpp>

namespace KinovaZED::Control {

struct CoreStateMachine {

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

		struct EStop : ActorEventBase<EStop> {
			auto operator()() const -> void;
		};

		struct QuitESTop : ActorEventBase<QuitESTop> {
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

	auto operator()() const noexcept {
		using namespace boost::sml;

		auto isSteeringMode = [](auto const &e) { return e.mode != Hw::SteeringMode::NoMode; };

		// clang-format off
		return make_transition_table(
		    // [poweredOff]
		    *"poweredOff"_s + event<Event::Initialize> / [](auto const &e) { e(); } = "initialized"_s,
		    "poweredOff"_s  + event<Event::EStop>      / [](auto const &e) { e(); }  = "emergencyStopped"_s,
			
			// [initialized]
			"initialized"_s + event<Event::Retract> / [](auto const &e) { e(); } = "retracting"_s,
		    "initialized"_s + event<Event::EStop>   / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [retracting]
			"retracting"_s + event<Event::Retracted>                              = "retracted"_s,
		    "retracting"_s + event<Event::EStop>     / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [retracted]
			"retracted"_s + event<Event::Unfold> / [](auto const &e) { e(); } = "unfolding"_s,
		    "retracted"_s + event<Event::EStop>  / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [unfolded]
			"unfolding"_s + event<Event::Unfolded>                              = "idle"_s,
		    "unfolding"_s + event<Event::EStop>    / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [idle]
			"idle"_s + event<Event::SetMode>      / [](auto const &e) { e(); } = "settingMode"_s,
			"idle"_s + event<Event::GoToPosition> / [](auto const &e) { e(); } = "runningSequence"_s,
			"idle"_s + event<Event::Retract>      / [](auto const &e) { e(); } = "retracting"_s,
		    "idle"_s + event<Event::EStop>        / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [settingMode]
			"settingMode"_s + event<Event::ModeSet> [ isSteeringMode ]                               = "steering"_s,
			"settingMode"_s + event<Event::ModeSet> [ !isSteeringMode ]                              = "idle"_s,
			"settingMode"_s + event<Event::Retract>                     / [](auto const &e) { e(); } = "retracting"_s,
		    "settingMode"_s + event<Event::EStop>                       / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [steering]
			"steering"_s + event<Event::SetMode>                                    = "settingMode"_s,
			"steering"_s + event<Event::JoystickMoved> / [](auto const &e) { e(); } = "steering"_s,
			"steering"_s + event<Event::GoToPosition>  / [](auto const &e) { e(); } = "runningSequence"_s,
			"steering"_s + event<Event::Retract>       / [](auto const &e) { e(); } = "retracting"_s,
		    "steering"_s + event<Event::EStop>         / [](auto const &e) { e(); } = "emergencyStopped"_s,

			// [runningSequence]
			"runningSequence"_s + event<Event::GoToPosition>  / [](auto const &e) { e(); } = "runningSequence"_s,
			"runningSequence"_s + event<Event::SequenceFinished>                           = "idle"_s,
		    "runningSequence"_s + event<Event::EStop>         / [](auto const &e) { e(); } = "emergencyStopped"_s,
			
		    // [emergencyStopped]
		    "emergencyStopped"_s + event<Event::QuitESTop> = "poweredOff"_s);
		// clang-format on
	}
};

} // namespace KinovaZED::Control

#endif