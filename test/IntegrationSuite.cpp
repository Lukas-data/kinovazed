#include "IntegrationSuite.h"
#include "cute.h"
#include "CommandHandling.h"
#include "Event.h"
#include "EventIOFake.h"
#include "RoboRioProtocol.h"
#include "../include/LogFile.h"
#include <thread>
#include <vector>

struct SequenceEvent {
	std::chrono::milliseconds delay;
	RoboRioProtocol::Packet command;
};

using namespace std::chrono_literals;

struct EventsForBellSequence {
	auto operator()() -> RoboRioProtocol::Packet {
		if (nextCommandIndex >= commands.size()) {
			std::this_thread::sleep_for(100ms);
			return RoboRioProtocol::Packet{KinovaFSM::Tick};
		}
		auto event = commands.at(nextCommandIndex++);
		std::this_thread::sleep_for(event.delay);
		return event.command;
	}
private:
	int nextCommandIndex{0};
	std::vector<SequenceEvent> commands{
		{1000ms, {KinovaFSM::Initialize}},
		{3000ms, {KinovaFSM::Tick}},
		{6000ms, {KinovaFSM::Initialized}},
		{3000ms, {KinovaFSM::Tick}},
		{7000ms, {KinovaFSM::Unfold}},
		{3000ms, {KinovaFSM::Tick}},
		{7000ms, {KinovaFSM::Unfolded}},
		{1000ms, {KinovaFSM::GoToPosition, 4}},

	};
};
template <typename EventFake>
void tickUntilEnd(CommandHandling<EventFake> & commandHandling) {
	for (int i = 0; i < 100; i++) {
		commandHandling.process();
	}
}

void thisIsAIntegrationSuiteTest() {
	LogFile::create();
	CommandHandling<EventIOFake<EventsForBellSequence>> commandHandling{};
	for (int i = 0; i < 8; i++) {
		commandHandling.process();
	}
	tickUntilEnd(commandHandling);
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
