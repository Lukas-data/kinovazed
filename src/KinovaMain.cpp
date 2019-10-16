
#include "LogFile.h"
#include "spdlog/spdlog.h"
#include "CommandHandling.h"

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

int main() {

	log = spdlog::get("debug")
	/*
	 * set log level here 
	 */
	log->set_level(spdlog::level::info);

	if (LogFile::create()) {
		debug_logger::info("KinovaMain - Startup!");
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
	}
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10s);
	return -1;
}

bool setup_logger(){

	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_sink_st>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("log.txt", 1048576 * 20, 200, true));
	auto debug_logger = std::make_shared<spdlog::logger>("debug", begin(sinks), end(sinks));
	spdlog::register_logger(debug_logger);
	
}