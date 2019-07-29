#include "IntegrationSuite.h"
#include "cute.h"
#include "CommandHandling.h"
#include "Event.h"
#include "EventIOFake.h"
#include "RoboRioProtocol.h"
#include "../include/LogFile.h"

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

struct SequenceEvent {
	std::chrono::milliseconds delay;
	RoboRioProtocol::Packet command;
	RoboRioProtocol::Packet reaction;
};

using namespace std::chrono_literals;

struct EventsForBellSequence {
	auto operator()() -> RoboRioProtocol::Packet {
		if (nextCommandIndex >= commands.size()) {
			std::this_thread::sleep_for(100ms);
			return RoboRioProtocol::Packet{KinovaFSM::Tick};
		}
		auto event = commands.at(nextCommandIndex);
		std::this_thread::sleep_for(event.delay);
		return event.command;
	}

	auto check(RoboRioProtocol::Packet const & reaction) -> bool{
		if (nextCommandIndex < commands.size()) {
			ASSERT_EQUAL(commands.at(nextCommandIndex).reaction, reaction);
			nextCommandIndex++;
		}
		if (reaction.command == KinovaFSM::SequenceDone) {
			//endReached = true;
		}
		return true;
	}

	template <typename EventFake>
	void tickUntilEnd(CommandHandling<EventFake> & commandHandling) {
		for (int i = 0; !endReached && i < 100; i++) {
			std::this_thread::sleep_for(500ms);
			commandHandling.process();
		}
	}

private:
	int nextCommandIndex{0};

	bool endReached = false;

	std::vector<SequenceEvent> commands{
		{200ms, {KinovaFSM::Initialize}, {KinovaFSM::Initialize}},
		{6000ms, {KinovaFSM::Tick}, {KinovaFSM::Initialized}},
		{5000ms, {KinovaFSM::Initialized}, {KinovaFSM::Retracted}},
		{1000ms, {KinovaFSM::Tick}, {KinovaFSM::Retracted}},
		{500ms, {KinovaFSM::Unfold}, {KinovaFSM::Unfold}},
		{500ms, {KinovaFSM::Tick}, {KinovaFSM::Unfold}},
		{5000ms, {KinovaFSM::Unfolded}, {KinovaFSM::NoEvent}},
		{2000ms, {KinovaFSM::GoToPosition, 4}, {KinovaFSM::GoToPosition, 4}},
	};
};


void thisIsAIntegrationSuiteTest() {
	LogFile::create();
	std::shared_ptr<EventsForBellSequence> sequence = std::make_shared<EventsForBellSequence>();
	auto eventIo = std::make_unique<EventIOFake<EventsForBellSequence>>(sequence);
	auto jacoArm = std::make_unique<KinovaArm>();
	CommandHandling<EventIOFake<EventsForBellSequence>> commandHandling{std::move(eventIo), std::move(jacoArm)};
	for (int i = 0; i < 8; i++) {
		commandHandling.process();
	}
	sequence->tickUntilEnd(commandHandling);
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
