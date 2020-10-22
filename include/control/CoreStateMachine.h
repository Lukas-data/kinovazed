#ifndef INCLUDE_CONTROL_CORE_STATE_MACHINE_H_
#define INCLUDE_CONTROL_CORE_STATE_MACHINE_H_

#include "hw/Actor.h"
#include "support/Logging.h"

#include <sml/sml.hpp>

#include <utility>

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

		struct Freeze : ActorEventBase<Freeze> {
			auto operator()() const -> void;
		};

		struct Thaw : ActorEventBase<Thaw> {
			auto operator()() const -> void;
		};

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

		struct SetJoystickMode : ActorEventBase<SetJoystickMode> {
			auto operator()() const -> void;

			Hw::SteeringMode mode;
		};

		struct RunObjective : ActorEventBase<RunObjective> {
			auto operator()() const -> void;

			Hw::Coordinates position;
		};

		struct ModeSet {
			Hw::SteeringMode mode;
		};

		struct JoystickMoved : ActorEventBase<JoystickMoved> {
			auto constexpr static absoluteLimitX = 0.8;
			auto constexpr static absoluteLimitY = 0.85;

			auto operator()() const -> void;

			int x;
			int y;
			int z;
		};

		struct SequenceFinished : ActorEventBase<JoystickMoved> {
			auto operator()() const -> void;
		};
	};

	explicit CoreStateMachine(Logger logger)
	    : LoggingMixin{logger, "CoreStateMachine"} {
	}

	static auto constexpr poweredOff = boost::sml::state<struct PoweredOffStateTag>;
	static auto constexpr initializing = boost::sml::state<struct InitializingStateTag>;
	static auto constexpr retracting = boost::sml::state<struct RetractingStateTag>;
	static auto constexpr retracted = boost::sml::state<struct RetractedStateTag>;
	static auto constexpr unfolding = boost::sml::state<struct UnfoldingStateTag>;
	static auto constexpr idle = boost::sml::state<struct IdleStateTag>;
	static auto constexpr settingMode = boost::sml::state<struct SettingModeStateTag>;
	static auto constexpr steering = boost::sml::state<struct SteeringStateTag>;
	static auto constexpr runningSequence = boost::sml::state<struct RunningSequenceStateTag>;
	static auto constexpr frozen = boost::sml::state<struct FrozenStateTag>;
	static auto constexpr emergencyStopped = boost::sml::state<struct EmergencyStoppedStateTag>;

	auto operator()() noexcept {
		using namespace boost::sml;
		using boost::sml::on_exit;

		auto isNoMode = [](auto const &e) { return e.mode == Hw::SteeringMode::NoMode; };

		auto isSteeringMode = [&](auto const &e) { return !isNoMode(e); };

		auto eventAction = [](auto const &e) { e(); };

		auto logEntry = [this](auto state) {
			return [this, state] { logDebug("<transition>", "entered state '{}'", state); };
		};

		auto logExit = [this](auto state) {
			return [this, state] { logDebug("<transition>", "left state '{}'", state); };
		};

		// clang-format off
		return make_transition_table(
		    poweredOff       + event<Event::Initialize>                 / eventAction = initializing,
			poweredOff       + event<Event::EStop>                      / eventAction = emergencyStopped,
			poweredOff       + event<Event::QuitEStop>                  / eventAction,              

			initializing     + event<Event::Initialized>                              = idle,
			initializing     + event<Event::EStop>                      / eventAction = emergencyStopped,

			retracting       + event<Event::Retracted>                                = retracted,
			retracting       + event<Event::EStop>                      / eventAction = emergencyStopped,

			retracted        + event<Event::Unfold>                     / eventAction = unfolding,
			retracted        + event<Event::EStop>                      / eventAction = emergencyStopped,

			unfolding        + event<Event::Unfolded>                                 = idle,
			unfolding        + event<Event::EStop>                      / eventAction = emergencyStopped,

			idle             + event<Event::SetJoystickMode>            / eventAction = settingMode,
			idle             + event<Event::RunObjective>               / eventAction = runningSequence,
			idle             + event<Event::Retract>                    / eventAction = retracting,
			idle             + event<Event::Initialize>                 / eventAction = initializing,
			idle             + event<Event::Freeze>                     / eventAction = frozen,
			idle             + event<Event::EStop>                      / eventAction = emergencyStopped,

			settingMode      + event<Event::ModeSet> [isSteeringMode]                 = steering,
			settingMode      + event<Event::ModeSet> [isNoMode]                       = idle,
			settingMode      + event<Event::Retract>                    / eventAction = retracting,
			settingMode      + event<Event::EStop>                      / eventAction = emergencyStopped,

			steering         + event<Event::SetJoystickMode>            / eventAction = settingMode,
			steering         + event<Event::JoystickMoved>              / eventAction,
			steering         + event<Event::RunObjective>               / eventAction = runningSequence,
			steering         + event<Event::Freeze>                     / eventAction = frozen,
			steering         + event<Event::EStop>                      / eventAction = emergencyStopped,

			runningSequence  + event<Event::RunObjective>               / eventAction = runningSequence,
			runningSequence  + event<Event::SequenceFinished>           / eventAction = idle,
			runningSequence  + event<Event::SetJoystickMode>            / eventAction = settingMode,
			runningSequence  + event<Event::EStop>                      / eventAction = emergencyStopped,

			frozen           + event<Event::EStop>                      / eventAction = emergencyStopped,
			frozen           + event<Event::Thaw>                       / eventAction = idle,

		   *emergencyStopped + event<Event::QuitEStop>                                = poweredOff,
			emergencyStopped + event<Event::EStop>                                    = emergencyStopped,

			poweredOff + on_entry<_>       / logEntry("poweredOff"),
			poweredOff + on_exit<_>        / logExit("poweredOff"),
			initializing + on_entry<_>     / logEntry("initializing"),
			initializing + on_exit<_>      / logExit("initializing"),
			retracting + on_entry<_>       / logEntry("retracting"),
			retracting + on_exit<_>        / logExit("retracting"),
			retracted + on_entry<_>        / logEntry("retracted"),
			retracted + on_exit<_>         / logExit("retracted"),
			unfolding + on_entry<_>        / logEntry("unfolding"),
			unfolding + on_exit<_>         / logExit("unfolding"),
			idle + on_entry<_>             / logEntry("idle"),
			idle + on_exit<_>              / logExit("idle"),
			settingMode + on_entry<_>      / logEntry("settingMode"),
			settingMode + on_exit<_>       / logExit("settingMode"),
			steering + on_entry<_>         / logEntry("steering"),
			steering + on_exit<_>          / logExit("steering"),
			runningSequence + on_entry<_>  / logEntry("runningSequence"),
			runningSequence + on_exit<_>   / logExit("runningSequence"),
			frozen + on_entry<_>           / logEntry("safe"),
			frozen + on_exit<_>            / logExit("safe"),
			emergencyStopped + on_entry<_> / logEntry("emergencyStopped"),
			emergencyStopped + on_exit<_>  / logExit("emergencyStopped")
		);
		// clang-format on
	}
};

} // namespace KinovaZED::Control

#endif