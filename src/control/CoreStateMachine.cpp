#include "control/CoreStateMachine.h"

#include "hw/Actor.h"

namespace KinovaZED::Control {

auto CoreStateMachine::Event::Initialize::operator()() const -> void {
	actor.takeControl();
}

auto CoreStateMachine::Event::EStop::operator()() const -> void {
	actor.stopMoving();
	actor.releaseControl();
}

auto CoreStateMachine::Event::Retract::operator()() const -> void {
	actor.retract();
}

auto CoreStateMachine::Event::Unfold::operator()() const -> void {
	actor.home();
}

auto CoreStateMachine::Event::SetMode::operator()() const -> void {
	actor.setSteeringMode(mode);
}

auto CoreStateMachine::Event::GoToPosition::operator()() const -> void {
	actor.moveTo(position);
}

auto CoreStateMachine::Event::JoystickMoved::operator()() const -> void {
	actor.setJoystick(x, y, z);
}

} // namespace KinovaZED::Control