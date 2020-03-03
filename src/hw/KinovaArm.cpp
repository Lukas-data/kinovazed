#include "hw/KinovaArm.h"

#include "hw/Coordinates.h"
#include "support/Logging.h"

#include <asio/io_context.hpp>

#include <future>

using namespace std::chrono_literals;

namespace KinovaZED::Hw {

constexpr auto positionRange = 0.05;
constexpr auto rotationRange = 0.25;
constexpr auto velocityRange = 0.000002;

KinovaArm::KinovaArm(Logger logger)
    : KinovaArm{std::nullopt, logger} {
}

KinovaArm::KinovaArm(Coordinates homePosition, Logger logger)
    : KinovaArm{std::optional{homePosition}, logger} {
}

KinovaArm::~KinovaArm() {
	disconnect();
	ioWorkGuard.reset();
	ioContext.stop();
	ioRunner.get();
}

} // namespace KinovaZED::Hw