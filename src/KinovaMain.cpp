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
		arm.retract();
		// arm.setSteeringMode(KinovaZED::Hw::SteeringMode::Rotation);
	}

	auto onRetractionPointReached(KinovaZED::Hw::Actor &arm) -> void {
		logger->info("ArmListener::onRetractionPointReached: the arm reached the retracted position");
		arm.home();
	}

	auto onSteeringModeChanged(KinovaZED::Hw::Actor &arm, KinovaZED::Hw::SteeringMode mode) -> void {
		logger->info("ArmListener::onSteeringModeChanged: the arm changed to steering mode {0}",
		             static_cast<int>(mode));
		(void)arm;
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
		arm.home();
	}


	// auto objectivesFile = std::ifstream{Paths::DEFAULT_OBJ_FILE_JSON};

	// auto io = std::make_unique<TCPServer>(logger);
	// auto arm = std::make_shared<KinovaArm>(objectivesFile, logger);
	// auto commandHandler = CommandHandling{std::move(io), arm, logger};

	// while (true) {
	// 	try {
	// 		commandHandler.process();
	// 	} catch (std::runtime_error const &e) {
	// 		logger->critical("RuntimeError: {}", e.what());
	// 		return -1;
	// 	} catch (std::exception const &e) {
	// 		logger->critical("Exception: {}", e.what());
	// 		return -1;
	// 	} catch (...) {
	// 		logger->critical("UNKNOWN ERROR");
	// 		return -1;
	// 	}
	// }

	std::this_thread::sleep_for(120s);

	// return -1;
}
