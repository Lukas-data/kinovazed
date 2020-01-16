// #include "control/CommandHandler.h"
#include "hw/KinovaArm.h"
#include "support/Logging.h"
#include "support/Paths.h"
#include "support/Prefix.h"

#include <chrono>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <thread>

struct ArmListener : KinovaZED::Hw::Actor::EventSubscriber {

	explicit ArmListener(KinovaZED::Logger logger)
	    : logger{logger} {
	}

	auto onHomeReached(KinovaZED::Hw::Actor &arm) -> void {
		logger->info("ArmListener::onHomeReached: the arm reached the home position");
		arm.setSteeringMode(KinovaZED::Hw::SteeringMode::Rotation);
	}

	auto onRetractionPointReached(KinovaZED::Hw::Actor &arm) -> void {
		logger->info("ArmListener::onRetractionPointReached: the arm reached the retracted position");
		arm.home();
	}

	auto onSteeringModeChanged(KinovaZED::Hw::Actor &arm, KinovaZED::Hw::SteeringMode mode) -> void {
		logger->info("ArmListener::onSteeringModeChanged: the arm changed to steering mode {0}",
		             static_cast<int>(mode));
		arm.setJoystick(4000, 4000, 4000);
	}

	auto onReconnectedDueToError(KinovaZED::Hw::Actor &arm) -> void {
		arm.home();
	}

  private:
	KinovaZED::Logger logger;
};

int main() {
	using namespace std::chrono_literals;
	using namespace KinovaZED::Literals;

	auto loggerConfig = KinovaZED::LogConfiguration{};

	loggerConfig.loggerName = "kinovaZED";

	loggerConfig.consoleConfiguration = KinovaZED::ConsoleLogConfiguration{
	    KinovaZED::ConsoleStream::StandardOutput,
	    KinovaZED::ColorPolicy::DoColor,
	};

	loggerConfig.fileConfiguration = KinovaZED::FileLogConfiguration{
	    KinovaZED::DEFAULT_LOG_FILE,
	    50_M,
	    200,
	    KinovaZED::RotationPolicy::DoRotateOnOpen,
	};

	auto logger = KinovaZED::makeLogger(loggerConfig);

	logger->set_level(spdlog::level::debug);

	logger->info("main: starting up");

	auto listener = std::make_shared<ArmListener>(logger);

	auto arm = KinovaZED::Hw::KinovaArm{logger};
	arm.setShouldReconnectOnError(true);

	arm.subscribe(listener);

	if (arm.connect()) {
		arm.takeControl();
		arm.stopMoving();
		arm.home();
	}

	std::this_thread::sleep_for(15s);
}
