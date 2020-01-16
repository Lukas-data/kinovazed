#ifndef INCLUDE_CONTROL_STATE_MACHINE_H_
#define INCLUDE_CONTROL_STATE_MACHINE_H_

#include "control/Event.h"
// #include "control/State.h"
#include "control/Transition.h"
#include "hw/KinovaArm.h"
#include "support/Logging.h"

#include <chrono>
#include <memory>

namespace KinovaZED::Control {

struct StateMachine {
	explicit StateMachine(Logger logger);

	// bool process(Event e, int var);

  private:
	Logger logger;
	struct State *currentState;
	int const numberOfTransitions;
	std::chrono::time_point<std::chrono::steady_clock> lastTick;
};

} // namespace KinovaZED::Control
#endif
