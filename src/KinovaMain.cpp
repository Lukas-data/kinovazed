#include "LogFile.h"
#include "Log.h"
#include "CommandHandling.h"

#include <chrono>
#include <stdexcept>
#include <thread>

int main() {
	if (LogFile::create()) {
		ALL_LOG(logINFO) << "KinovaMain - Startup!";
		CommandHandling<> commandHandler { };
		while (true) {
			try {
				commandHandler.process();
			} catch (std::runtime_error const &e) {
				ALL_LOG(logERROR) << "RuntimeError: " << e.what();
				return -1;
			}
		}
	}
	using namespace std::chrono_literals;
	std::this_thread::sleep_for(10s);
	return -1;
}
