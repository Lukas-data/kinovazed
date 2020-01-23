#ifndef INCLUDE_CONTROL_HEARTBEAT_GENERATOR_H_
#define INCLUDE_CONTROL_HEARTBEAT_GENERATOR_H_

#include "comm/CommandInterface.h"
#include "support/Logging.h"

#include <asio/io_context.hpp>
#include <asio/steady_timer.hpp>

namespace KinovaZED::Control {

struct HeartbeatGenerator : LoggingMixin {
	HeartbeatGenerator(Comm::CommandInterface &sink, asio::io_context &timerContext, Logger logger);

	~HeartbeatGenerator() noexcept;

	auto start() -> void;
	auto stop() -> void;

  private:
	auto beat() -> void;

	Comm::CommandInterface &sink;
	asio::io_context &timerContext;
	asio::steady_timer timer{timerContext};
};

} // namespace KinovaZED::Control

#endif