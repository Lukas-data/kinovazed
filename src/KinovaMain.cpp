
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CommandHandling.h"
#include "Prefix.h"

#include <chrono>
#include <exception>
#include <memory>
#include <stdexcept>
#include <thread>
#include <string>

namespace Constants {
	/*
	 * Logging
	 */
	auto constexpr LOG_FILE = "logs/log.txt";
	auto constexpr LOGGER_NAME = "robolog";

	/*
	 * JSON keys
	 */
	auto constexpr OBJ_NAME = "name";
	auto constexpr OBJ_ORIGIN_ABS = "is_abs";
	auto constexpr OBJ_ORIGIN = "origin";
	auto constexpr OBJ_SEQUENCE = "sequence";
	auto constexpr OBJ_CSYS_X = "X";
	auto constexpr OBJ_CSYS_Y = "Y";
	auto constexpr OBJ_CSYS_Z = "Z";
	auto constexpr OBJ_CSYS_PITCH = "pitch";
	auto constexpr OBJ_CSYS_YAW = "yaw";
	auto constexpr OBJ_CSYS_ROLL = "roll";

}

void setup_logger(std::string name) {
	try {
		auto console_sink { std::make_shared<spdlog::sinks::stdout_color_sink_st>() };
		console_sink->set_level(spdlog::level::info);

		using namespace BytePrefix;
		auto file_sink { std::make_shared<spdlog::sinks::rotating_file_sink_mt>(Constants::LOG_FILE, 50_M, 200, true) };
		file_sink->set_level(spdlog::level::trace);

		auto logger { std::make_shared<spdlog::logger>(name, spdlog::sinks_init_list { console_sink, file_sink }) };
		logger->set_level(spdlog::level::trace);
		spdlog::register_logger(logger);
		spdlog::set_default_logger(logger);
		spdlog::flush_on(spdlog::level::warn);
	} catch (const spdlog::spdlog_ex &ex) {
		spdlog::critical("Log initialization failed: {}", ex.what());
	}
}

int main() {
	setup_logger(Constants::LOGGER_NAME);
	spdlog::info("KinovaMain - Startup!");
	CommandHandling<> commandHandler { };
	while (true) {
		try {
			commandHandler.process();
		} catch (std::runtime_error const &e) {
			spdlog::critical("RuntimeError: {}", e.what());
			return -1;
		} catch (std::exception const &e) {
			spdlog::critical("Exception: {}", e.what());
			return -1;
		} catch (...) {
			spdlog::critical("UNKNOWN ERROR");
			return -1;
		}
	}
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10s);
	return -1;

}

