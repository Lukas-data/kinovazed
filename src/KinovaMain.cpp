
#include "LogFile.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "CommandHandling.h"

#include <chrono>
#include <exception>
#include <stdexcept>
#include <thread>

int main() {

	setup_logger();
	auto log = spdlog::get("robolog");

	if(log) {
		/*
		* set log level here
		*/
		log->set_level(spdlog::level::info);
	}
	else {
		spdlog::error("create logger failed!");
	}

	if (LogFile::create()) {
		log->info("KinovaMain - Startup!");
		CommandHandling<> commandHandler { };
		while (true) {
			try {
				commandHandler.process();
			} catch (std::runtime_error const &e) {
				log->error("RuntimeError: {}", e.what());
				return -1;
			} catch (std::exception const &e) {
				log->error("Exception: {}", e.what());
				return -1;
			} catch (...) {
				log->error("UNKNOWN ERROR");
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
	sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>("filelog", "log.txt", 1048576 * 20, 200, true));
	auto debug_logger = std::make_shared<spdlog::logger>("robolog", begin(sinks), end(sinks));
	spdlog::register_logger(debug_logger);
}