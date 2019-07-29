#include "IntegrationSuite.h"
#include "cute.h"
#include "CommandHandling.h"
#include "Event.h"
#include "EventIOFake.h"
#include "RoboRioProtocol.h"
#include "../include/LogFile.h"

#include <chrono>
#include <string>
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
		{200ms, {KinovaFSM::Initialize}},
		{6000ms, {KinovaFSM::Tick}},
		{5000ms, {KinovaFSM::Initialized}},
		{200ms, {KinovaFSM::Tick}},
		{200ms, {KinovaFSM::Unfold}},
		{200ms, {KinovaFSM::Tick}},
		{5000ms, {KinovaFSM::Unfolded}},
		{200ms, {KinovaFSM::GoToPosition, 4}},

	};
};
template <typename EventFake>
void tickUntilEnd(CommandHandling<EventFake> & commandHandling) {
	for (int i = 0; i < 300; i++) {
		commandHandling.process();
	}
}

void thisIsAIntegrationSuiteTest() {
	LogFile::create();
	CommandHandling<EventIOFake<EventsForBellSequence>> commandHandling{};
	for (int i = 0; i < 8; i++) {
		std::string dummy{};
		commandHandling.process();
	}
	tickUntilEnd(commandHandling);
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
