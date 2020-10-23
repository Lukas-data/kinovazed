#include "control/CoreStateMachine.h"

#include "hw/Actor.h"

namespace KinovaZED::Control {

auto CoreStateMachine::Event::Initialize::operator()() const -> void {
	if (actor.takeControl()) {
		actor.initialize();
	}
}

auto CoreStateMachine::Event::Freeze::operator()() const -> void {
	actor.stopMoving();
}

auto CoreStateMachine::Event::Thaw::operator()() const -> void {
}

auto CoreStateMachine::Event::EStop::operator()() const -> void {
	actor.stopMoving();
	actor.releaseControl();
}

auto CoreStateMachine::Event::QuitEStop::operator()() const -> void {
}

auto CoreStateMachine::Event::Retract::operator()() const -> void {
	actor.retract();
}

auto CoreStateMachine::Event::Unfold::operator()() const -> void {
	actor.home();
}

auto CoreStateMachine::Event::SetJoystickMode::operator()() const -> void {
	actor.stopMoving();
	actor.setSteeringMode(mode);
}

auto CoreStateMachine::Event::RunObjective::operator()() const -> void {
	actor.stopMoving();
	actor.moveTo(position);
}

auto CoreStateMachine::Event::JoystickMoved::operator()() const -> void {
	auto currentPosition = actor.getPosition();
	auto boundedX = x, boundedY = y;

	if ((x < 0 && currentPosition.x <= -absoluteLimitX) || (x > 0 && currentPosition.x >= absoluteLimitX)) {
		boundedX = 0;
	}

	if ((y < 0 && currentPosition.y <= -absoluteLimitY) || (y > 0 && currentPosition.y >= absoluteLimitY)) {
		boundedY = 0;
	}

	actor.setJoystick(boundedX, boundedY, z);
}

auto CoreStateMachine::Event::SequenceFinished::operator()() const -> void {
	actor.stopMoving();
}

} // namespace KinovaZED::Control