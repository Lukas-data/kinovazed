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

		struct GoToSafe : ActorEventBase<GoToSafe> {
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
	static auto constexpr runningSafeSequence = boost::sml::state<struct RunningSafeSequenceStateTag>;
	static auto constexpr safe = boost::sml::state<struct SafeStateTag>;
	static auto constexpr emergencyStopped = boost::sml::state<struct EmergencyStoppedStateTag>;

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
			*poweredOff + event<Event::Initialize> / eventAction = initializing,
			poweredOff  + event<Event::EStop>      / eventAction = emergencyStopped,
			poweredOff  + on_entry<_>              / logEntry("poweredOff"),
			poweredOff  + on_exit<_>               / logExit("poweredOff"),
			
			// [poweredOff]
			initializing + event<Event::Initialized>               = idle,
			initializing + event<Event::EStop>       / eventAction = emergencyStopped,
			initializing + on_entry<_>               / logEntry("initializing"),
			initializing + on_exit<_>                / logExit("initializing"),

			// [retracting]
			retracting + event<Event::Retracted>               = retracted,
			retracting + event<Event::EStop>     / eventAction = emergencyStopped,
			retracting + on_entry<_>             / logEntry("retracting"),
			retracting + on_exit<_>              / logExit("retracting"),

			// [retracted]
			retracted + event<Event::Unfold> / eventAction = unfolding,
			retracted + event<Event::EStop>  / eventAction = emergencyStopped,
			retracted + on_entry<_>          / logEntry("retracted"),
			retracted + on_exit<_>           / logExit("retracted"),

			// [unfolded]
			unfolding + event<Event::Unfolded>               = idle,
			unfolding + event<Event::EStop>    / eventAction = emergencyStopped,
			unfolding + on_entry<_>            / logEntry("unfolding"),
			unfolding + on_exit<_>             / logExit("unfolding"),

			// [idle]
			idle + event<Event::SetMode>      / eventAction = settingMode,
			idle + event<Event::GoToPosition> / eventAction = runningSequence,
			idle + event<Event::GoToSafe>     / eventAction = runningSafeSequence,
			idle + event<Event::Retract>      / eventAction = retracting,
			idle + event<Event::Unfold>       / eventAction = unfolding,
			idle + event<Event::EStop>        / eventAction = emergencyStopped,
			idle + on_entry<_>                / logEntry("idle"),
			idle + on_exit<_>                 / logExit("idle"),

			// [settingMode]
			settingMode + event<Event::ModeSet> [ isSteeringMode ]                = steering,
			settingMode + event<Event::ModeSet> [ !isSteeringMode ]               = idle,
			settingMode + event<Event::Retract>                     / eventAction = retracting,
			settingMode + event<Event::EStop>                       / eventAction = emergencyStopped,
			settingMode + on_entry<_>                               / logEntry("settingMode"),
			settingMode + on_exit<_>                                / logExit("settingMode"),

			// [steering]
			steering + event<Event::SetMode>       / eventAction = settingMode,
			steering + event<Event::JoystickMoved> / eventAction = steering,
			steering + event<Event::GoToPosition>  / eventAction = runningSequence,
			steering + event<Event::Retract>       / eventAction = retracting,
			steering + event<Event::EStop>         / eventAction = emergencyStopped,
			steering + on_entry<_>                 / logEntry("steering"),
			steering + on_exit<_>                  / logExit("steering"),

			// [runningSequence]
			runningSequence + event<Event::GoToPosition>     / eventAction = runningSequence,
			runningSequence + event<Event::SequenceFinished> / eventAction = idle,
			runningSequence + event<Event::EStop>            / eventAction = emergencyStopped,
			runningSequence + on_entry<_>                    / logEntry("runningSequence"),
			runningSequence + on_exit<_>                     / logExit("runningSequence"),

			// [runningSafeSequence]
			runningSafeSequence + event<Event::GoToSafe>         / eventAction = runningSafeSequence,
			runningSafeSequence + event<Event::SequenceFinished> / eventAction = safe,
			runningSafeSequence + event<Event::EStop>            / eventAction = emergencyStopped,
			runningSafeSequence + on_entry<_>                    / logEntry("runningSafeSequence"),
			runningSafeSequence + on_exit<_>                     / logExit("runningSafeSequence"),

			// [safe]
			safe + event<Event::EStop>  / eventAction = emergencyStopped,
			safe + event<Event::Unfold> / eventAction = unfolding,
			safe + on_entry<_>          / logEntry("safe"),
			safe + on_exit<_>           / logExit("safe"),

		    // [emergencyStopped]
			emergencyStopped + event<Event::QuitEStop> = poweredOff,
			emergencyStopped + event<Event::EStop>     = emergencyStopped,
			emergencyStopped + on_entry<_>             / logEntry("emergencyStopped"),
			emergencyStopped + on_exit<_>              / logExit("emergencyStopped")
		    // clang-format on
		);
	}
};

} // namespace KinovaZED::Control

#endif