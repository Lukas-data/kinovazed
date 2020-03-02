#include "comm/TCPInterface.h"
#include "control/CoreController.h"
#include "control/HeartbeatGenerator.h"
#include "control/ObjectiveManager.h"
#include "hw/KinovaArm.h"
#include "support/LineCommandFactory.h"
#include "support/Logging.h"
#include "support/Paths.h"
#include "support/Prefix.h"

#include <asio/io_context.hpp>

#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <thread>

auto constexpr DEFAULT_PORT = 51717;
auto constexpr MAXIMUM_RETRIES = 120;

auto makeLogger() -> KinovaZED::Logger {
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

	return KinovaZED::makeLogger(loggerConfig);
}

int main() {
	using namespace std::chrono_literals;

	auto logger = makeLogger();
	logger->set_level(spdlog::level::debug);
	logger->info("main: starting up");

	auto arm = KinovaZED::Hw::KinovaArm{logger};
	auto retry{0};

	while (!arm.connect() && retry++ < MAXIMUM_RETRIES) {
		std::this_thread::sleep_for(1s);
	}

	if (retry >= MAXIMUM_RETRIES) {
		logger->error("main: arm did not connect in time.");
		return EXIT_FAILURE;
	}

	arm.setShouldReconnectOnError(true);

	auto ioContext = asio::io_context{};
	auto interface = KinovaZED::Comm::TCPInterface{KinovaZED::lineCommandFactory, ioContext, DEFAULT_PORT, logger};
	auto objectiveStream = std::ifstream{KinovaZED::DEFAULT_OBJ_FILE_JSON};
	auto objectiveManager = KinovaZED::Control::ObjectiveManager{objectiveStream, logger};
	auto controller = KinovaZED::Control::makeCoreController(interface, arm, objectiveManager, logger);
	auto heart = KinovaZED::Control::HeartbeatGenerator{interface, controller, ioContext, logger};

	interface.start();
	heart.start();

	ioContext.run();
}
