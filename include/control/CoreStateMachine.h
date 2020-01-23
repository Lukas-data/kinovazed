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

	auto operator()() const noexcept {
		using namespace boost::sml;

		auto isSteeringMode = [](auto const &e) { return e.mode != Hw::SteeringMode::NoMode; };

		auto eventAction = [](auto const &e) { e(); };

		// clang-format off
		return make_transition_table(
		    // [poweredOff]
		    *"poweredOff"_s + event<Event::Initialize> / eventAction = "initializing"_s,
		    "poweredOff"_s  + event<Event::EStop>      / eventAction = "emergencyStopped"_s,
			
		    // [poweredOff]
		    "initializing"_s + event<Event::Initialized>               = "initialized"_s,
		    "initializing"_s  + event<Event::EStop>      / eventAction = "emergencyStopped"_s,

			// [initialized]
			"initialized"_s + event<Event::Retract> / eventAction = "retracting"_s,
		    "initialized"_s + event<Event::EStop>   / eventAction = "emergencyStopped"_s,

			// [retracting]
			"retracting"_s + event<Event::Retracted>               = "retracted"_s,
		    "retracting"_s + event<Event::EStop>     / eventAction = "emergencyStopped"_s,

			// [retracted]
			"retracted"_s + event<Event::Unfold> / eventAction = "unfolding"_s,
		    "retracted"_s + event<Event::EStop>  / eventAction = "emergencyStopped"_s,

			// [unfolded]
			"unfolding"_s + event<Event::Unfolded>               = "idle"_s,
		    "unfolding"_s + event<Event::EStop>    / eventAction = "emergencyStopped"_s,

			// [idle]
			"idle"_s + event<Event::SetMode>      / eventAction = "settingMode"_s,
			"idle"_s + event<Event::GoToPosition> / eventAction = "runningSequence"_s,
			"idle"_s + event<Event::Retract>      / eventAction = "retracting"_s,
		    "idle"_s + event<Event::EStop>        / eventAction = "emergencyStopped"_s,

			// [settingMode]
			"settingMode"_s + event<Event::ModeSet> [ isSteeringMode ]                = "steering"_s,
			"settingMode"_s + event<Event::ModeSet> [ !isSteeringMode ]               = "idle"_s,
			"settingMode"_s + event<Event::Retract>                     / eventAction = "retracting"_s,
		    "settingMode"_s + event<Event::EStop>                       / eventAction = "emergencyStopped"_s,

			// [steering]
			"steering"_s + event<Event::SetMode>                     = "settingMode"_s,
			"steering"_s + event<Event::JoystickMoved> / eventAction = "steering"_s,
			"steering"_s + event<Event::GoToPosition>  / eventAction = "runningSequence"_s,
			"steering"_s + event<Event::Retract>       / eventAction = "retracting"_s,
		    "steering"_s + event<Event::EStop>         / eventAction = "emergencyStopped"_s,

			// [runningSequence]
			"runningSequence"_s + event<Event::GoToPosition>  / eventAction = "runningSequence"_s,
			"runningSequence"_s + event<Event::SequenceFinished>            = "idle"_s,
		    "runningSequence"_s + event<Event::EStop>         / eventAction = "emergencyStopped"_s,
			
		    // [emergencyStopped]
		    "emergencyStopped"_s + event<Event::QuitEStop> = "poweredOff"_s);
		// clang-format on
	}
};

} // namespace KinovaZED::Control

#endif