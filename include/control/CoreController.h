#ifndef INCLUDE_CONTROL_COMMAND_HANDLING_H_
#define INCLUDE_CONTROL_COMMAND_HANDLING_H_

#include "comm/CommandInterface.h"
#include "comm/CommandSubscriber.h"
#include "control/CoreStateMachine.h"
#include "control/Objective.h"
#include "control/ObjectiveManager.h"
#include "hw/Actor.h"
#include "support/Logging.h"

#include <sml/sml.hpp>

#include <bitset>
#include <memory>
#include <optional>
#include <string>

constexpr int numberOfJoystickMoveInputs = 3;

namespace KinovaZED::Control {

struct CoreController : std::enable_shared_from_this<CoreController>,
                        LoggingMixin,
                        Comm::CommandSubscriber,
                        Hw::Actor::EventSubscriber {

	auto process(Comm::Command command) -> void;

	auto onPositionReached(Hw::Actor &who, Hw::Coordinates point) -> void;
	auto onHomeReached(Hw::Actor &who) -> void;
	auto onRetractionPointReached(Hw::Actor &who) -> void;
	auto onSteeringModeChanged(Hw::Actor &who, Hw::SteeringMode mode) -> void;
	auto onReconnectedDueToError(Hw::Actor &who) -> void;
	auto onInitializationFinished(Hw::Actor &who) -> void;

	auto getSystemState() -> std::bitset<8>;

  protected:
	CoreController(Comm::CommandInterface &interface,
	               Hw::Actor &actor,
	               ObjectiveManager &objectiveManager,
	               Logger logger);

	Comm::CommandInterface &commandSource;
	Hw::Actor &arm;
	ObjectiveManager &objectiveManager;
	boost::sml::sm<CoreStateMachine> stateMachine;

  private:
	auto makeLoggedStepper(std::string function) {
		return [this, function = std::move(function)](auto event, std::string stepMessage, std::string failureMessage) {
			logInfo(function, stepMessage);
			auto didAccept = stateMachine.process_event(event);
			if (!didAccept) {
				logWarning(function, failureMessage);
			}
			return didAccept;
		};
	}

	std::optional<Objective> currentObjective{};
	bool isInitialized{};
};

auto makeCoreController(Comm::CommandInterface &interface,
                        Hw::Actor &actor,
                        ObjectiveManager &objectiveManager,
                        Logger logger) -> std::shared_ptr<CoreController>;

} // namespace KinovaZED::Control
#endif
