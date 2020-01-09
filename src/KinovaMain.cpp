#include "CommandHandling.h"
#include "Logging.h"
#include "Paths.h"
#include "Prefix.h"

#include <chrono>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <thread>

int main() {
	using namespace std::chrono_literals;
	using namespace BytePrefix;

	auto loggerConfig = Logging::LogConfiguration{};

	loggerConfig.loggerName = "kinovaZED";

	loggerConfig.consoleConfiguration = Logging::ConsoleLogConfiguration{
	    Logging::ConsoleStream::StandardOutput,
	    Logging::ColorPolicy::DoColor,
	};

	loggerConfig.fileConfiguration = Logging::FileLogConfiguration{
	    Paths::DEFAULT_LOG_FILE,
	    50_M,
	    200,
	    Logging::RotationPolicy::DoRotateOnOpen,
	};

	auto logger = Logging::makeLogger(loggerConfig);

	logger->info("KinovaMain - Startup!");

	auto objectivesFile = std::ifstream{Paths::DEFAULT_OBJ_FILE_JSON};

	auto io = std::make_unique<TCPServer>(logger);
	auto arm = std::make_shared<KinovaArm>(objectivesFile, logger);
	auto commandHandler = CommandHandling{std::move(io), arm, logger};

	while (true) {
		try {
			commandHandler.process();
		} catch (std::runtime_error const &e) {
			logger->critical("RuntimeError: {}", e.what());
			return -1;
		} catch (std::exception const &e) {
			logger->critical("Exception: {}", e.what());
			return -1;
		} catch (...) {
			logger->critical("UNKNOWN ERROR");
			return -1;
		}
	}

	std::this_thread::sleep_for(10s);

	return -1;
}
