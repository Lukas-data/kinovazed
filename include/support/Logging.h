#ifndef INCLUDE_SUPPORT_LOGGING_H_
#define INCLUDE_SUPPORT_LOGGING_H_

#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <ostream>
#include <string>

namespace KinovaZED {

enum struct FlushPolicy : bool { DontForce = false, DoForce = true };

enum struct ColorPolicy : bool { DontColor = false, DoColor = true };

enum struct RotationPolicy : bool { DontRotateOnOpen = false, DoRotateOnOpen = true };

enum struct ConsoleStream : bool { StandardOutput, StandardError };

using Logger = std::shared_ptr<spdlog::logger>;

struct StreamLogConfiguration {
	std::ostream &stream;
	FlushPolicy flushPolicy;
};

struct ConsoleLogConfiguration {
	ConsoleStream stream;
	ColorPolicy colorPolicy;
};

struct FileLogConfiguration {
	std::filesystem::path logFilePath;
	std::size_t maximumFileSize;
	std::size_t maximumFileCount;
	RotationPolicy rotationPolicy;
};

struct LogConfiguration {
	std::string loggerName;

	std::optional<StreamLogConfiguration> streamConfiguration;
	std::optional<ConsoleLogConfiguration> consoleConfiguration;
	std::optional<FileLogConfiguration> fileConfiguration;
};

auto makeLogger(LogConfiguration config) -> Logger;

struct LoggingMixin {
	explicit LoggingMixin(Logger logger, std::string className)
	    : logger{logger}
	    , className{className} {
	}

  protected:
	template<typename... ParameterTypes>
	auto logDebug(std::string functionName, std::string format, ParameterTypes &&... params) -> void {
		auto message = className + "::" + functionName + ": " + format;
		logger->debug(message, std::forward<ParameterTypes>(params)...);
	}

	template<typename... ParameterTypes>
	auto logInfo(std::string functionName, std::string format, ParameterTypes &&... params) -> void {
		auto message = className + "::" + functionName + ": " + format;
		logger->info(message, std::forward<ParameterTypes>(params)...);
	}

	template<typename... ParameterTypes>
	auto logWarning(std::string functionName, std::string format, ParameterTypes &&... params) -> void {
		auto message = className + "::" + functionName + ": " + format;
		logger->warn(message, std::forward<ParameterTypes>(params)...);
	}

	template<typename... ParameterTypes>
	auto logError(std::string functionName, std::string format, ParameterTypes &&... params) -> void {
		auto message = className + "::" + functionName + ": " + format;
		logger->error(message, std::forward<ParameterTypes>(params)...);
	}

  private:
	Logger logger;
	std::string className;
};

} // namespace KinovaZED

#endif