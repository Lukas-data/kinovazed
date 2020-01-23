#include "control/HeartbeatGenerator.h"

#include "comm/CommandInterface.h"
#include "support/Logging.h"

#include <asio/error.hpp>
#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

#include <chrono>

namespace KinovaZED::Control {

HeartbeatGenerator::HeartbeatGenerator(Comm::CommandInterface &sink, asio::io_context &timerContext, Logger logger)
    : LoggingMixin{logger, "HeartbeatGenerator"}
    , sink{sink}
    , timerContext{timerContext} {
}

HeartbeatGenerator::~HeartbeatGenerator() noexcept {
	stop();
}

auto HeartbeatGenerator::start() -> void {
	beat();
}

auto HeartbeatGenerator::stop() -> void {
	auto ignored = asio::error_code{};
	timer.cancel(ignored);
}

auto HeartbeatGenerator::beat() -> void {
	using namespace std::chrono_literals;

	timer.expires_from_now(1s);
	timer.async_wait([this](auto error) {
		if (error && error != asio::error::operation_aborted) {
			logError(
			    "<beat::lambda>", "error while executing timer. code: {}, reason: {}", error.value(), error.message());
			return;
		}

		logDebug("<beat::lambda>", "sending heartbeat");
		// sink.send(...)
		beat();
	});
}

} // namespace KinovaZED::Control