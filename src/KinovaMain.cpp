#include "comm/TCPInterface.h"
#include "control/CommandHandler.h"
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

	if (!arm.connect()) {
		return EXIT_FAILURE;
	}

	auto ioContext = asio::io_context{};
	auto interface = KinovaZED::Comm::TCPInterface{KinovaZED::lineCommandFactory, ioContext, 12345, logger};
	auto objectiveStream = std::ifstream{KinovaZED::DEFAULT_OBJ_FILE_JSON};
	auto objectiveManager = KinovaZED::Control::ObjectiveManager{objectiveStream, logger};
	auto commandHandler = KinovaZED::Control::makeCommandHandler(interface, arm, objectiveManager, logger);

	interface.start();

	ioContext.run();
}
