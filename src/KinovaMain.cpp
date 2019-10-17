
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CommandHandling.h"

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

namespace Constants {
	auto constexpr LOG_FILE = "logs/log.txt";
}

bool setup_logger(){
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(Constants::LOG_FILE, 1048576 * 20, 200, true));
	auto debug_logger = std::make_shared<spdlog::logger>("robolog", begin(sinks), end(sinks));
	spdlog::register_logger(debug_logger);
	spdlog::set_default_logger(debug_logger);
	return true;
}

int main() {

	setup_logger();

	spdlog::set_level(spdlog::level::info);

	/*
		* run with active logger
		*/
	spdlog::info("KinovaMain - Startup!");
	CommandHandling<> commandHandler { };
	while (true) {
		try {
			commandHandler.process();
		} catch (std::runtime_error const &e) {
			spdlog::error("RuntimeError: {}", e.what());
			return -1;
		} catch (std::exception const &e) {
			spdlog::error("Exception: {}", e.what());
			return -1;
		} catch (...) {
			spdlog::error("UNKNOWN ERROR");
			return -1;
		}
	}
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10s);
	return -1;

}

