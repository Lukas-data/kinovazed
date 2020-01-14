#ifndef INCLUDE_SUPPORT_LOGGING_H_
#define INCLUDE_SUPPORT_LOGGING_H_

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

} // namespace KinovaZED

#endif