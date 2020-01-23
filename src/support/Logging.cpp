#include "support/Logging.h"

#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace KinovaZED {

namespace {

auto makeStreamSink(std::optional<StreamLogConfiguration> config) -> spdlog::sink_ptr {
	if (!config) {
		return nullptr;
	}
	auto realConfiguration = config.value();
	return std::make_shared<spdlog::sinks::ostream_sink_mt>(realConfiguration.stream,
	                                                        static_cast<bool>(realConfiguration.flushPolicy));
}

auto makeFileSink(std::optional<FileLogConfiguration> config) -> spdlog::sink_ptr {
	if (!config) {
		return nullptr;
	}
	auto realConfiguration = config.value();
	return std::make_shared<spdlog::sinks::rotating_file_sink_mt>(realConfiguration.logFilePath.native(),
	                                                              realConfiguration.maximumFileSize,
	                                                              realConfiguration.maximumFileCount,
	                                                              static_cast<bool>(realConfiguration.rotationPolicy));
}

auto makeConsoleSink(std::optional<ConsoleLogConfiguration> config) -> spdlog::sink_ptr {
	if (!config) {
		return {};
	}
	auto realConfiguration = config.value();
	auto colorMode = realConfiguration.colorPolicy == ColorPolicy::DoColor ? spdlog::color_mode::automatic
	                                                                       : spdlog::color_mode::never;

	if (realConfiguration.stream == ConsoleStream::StandardOutput) {
		return std::make_shared<spdlog::sinks::stdout_color_sink_mt>(colorMode);
	}
	return std::make_shared<spdlog::sinks::stderr_color_sink_mt>(colorMode);
}

} // namespace

auto makeLogger(LogConfiguration config) -> Logger {
	auto sinks = std::vector{
	    makeStreamSink(config.streamConfiguration),
	    makeFileSink(config.fileConfiguration),
	    makeConsoleSink(config.consoleConfiguration),
	};
	auto logger = std::make_shared<spdlog::logger>(config.loggerName);

	std::for_each(cbegin(sinks), cend(sinks), [&logger](auto sinkPtr) {
		if (sinkPtr) {
			logger->sinks().push_back(sinkPtr);
		}
	});

	return logger;
}

} // namespace KinovaZED