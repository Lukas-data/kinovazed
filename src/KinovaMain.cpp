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

	auto arm = KinovaZED::Hw::KinovaArm{logger};
	arm.setShouldReconnectOnError(true);

	if (arm.connect()) {
		arm.takeControl();
		arm.home();
		std::this_thread::sleep_for(2s);
		arm.setSteeringMode(KinovaZED::Hw::SteeringMode::Rotation);
		std::this_thread::sleep_for(1s);
		arm.setJoystick(1000, 1000, 1000);
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

	std::this_thread::sleep_for(10s);

	// return -1;
}
