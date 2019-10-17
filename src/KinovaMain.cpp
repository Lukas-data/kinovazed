
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
	try{
		auto console_sink { std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };
		console_sink->set_level(spdlog::level::info);

		auto file_sink { std::make_shared<spdlog::sinks::rotating_file_sink_mt>(Constants::LOG_FILE, 1048576 * 20, 200, true) };
		file_sink->set_level(spdlog::level::trace);
		
		spdlog::logger logger("robolog", { console_sink, file_sink });
		spdlog::register_logger(logger);
		spdlog::set_default_logger(logger);
		return true;
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
		return false
	}
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

