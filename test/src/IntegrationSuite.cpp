#include "IntegrationSuite.h"

#include "EventIOFake.h"
#include "comm/TCPInterface.h"
#include "control/CommandHandling.h"
#include "control/Event.h"
#include "support/Logging.h"

#include <cute/cute.h>

#include <chrono>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

std::string const exampleData{
    R"data(
[
  {
    "name": "Home",
    "is_abs": true,
    "origin": { "X":  0.000, "Y": -0.400, "Z":  0.500, "pitch":  1.571, "yaw":  0.000, "roll":  0.000 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 }
    ]
  },
  {
    "name": "Bell",
    "is_abs": true,
    "origin": { "X": -0.461, "Y": -0.267, "Z":  0.525, "pitch": -1.989, "yaw": -1.418, "roll":  2.632 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z": -0.200, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
      { "X":  0.000, "Y":  0.000, "Z": -0.100, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 }
    ]
  },
  {
    "name": "Handle",
    "is_abs": true,
    "origin": { "X":  0.120, "Y": -0.723, "Z":  0.551, "pitch":  1.491, "yaw": -0.066, "roll":  0.002 },
    "sequence": [
      { "X": -0.031, "Y":  0.085, "Z": -0.175, "pitch": -0.027, "yaw": -0.028, "roll":  0.020 },
      { "X": -0.040, "Y":  0.080, "Z": -0.025, "pitch": -0.009, "yaw": -0.068, "roll":  0.006 }
    ]
  },
  {
    "name": "OpenDoor",
    "is_abs": false,
    "origin": { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
      { "X": -0.003, "Y":  0.008, "Z":  0.000, "pitch": -0.027, "yaw": -0.008, "roll":  0.713 },
      { "X": -0.033, "Y":  0.015, "Z": -0.197, "pitch": -0.060, "yaw":  0.192, "roll":  0.598 },
      { "X": -0.063, "Y":  0.000, "Z": -0.190, "pitch": -0.059, "yaw":  0.069, "roll": -1.543 },
      { "X": -0.070, "Y":  0.083, "Z": -0.188, "pitch": -0.053, "yaw":  0.055, "roll": -1.555 },
      { "X":  0.171, "Y":  0.030, "Z": -0.319, "pitch": -0.006, "yaw":  0.614, "roll": -0.106 },
      { "X":  0.163, "Y": -0.191, "Z": -0.039, "pitch":  0.023, "yaw":  0.216, "roll": -0.125 }
    ]
  },
  {
    "name": "PullDoor",
    "is_abs": false,
    "origin": { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
      { "X": -0.450, "Y": -0.080, "Z": -0.272, "pitch":  1.006, "yaw": -1.298, "roll":  0.902 },
      { "X": -0.576, "Y": -0.031, "Z": -0.375, "pitch":  1.013, "yaw": -1.427, "roll":  0.919 }
    ]
  },
  {
    "name": "PlaceCup",
    "is_abs": true,
    "origin": {"X": -0.705, "Y": -0.028, "Z":  0.755, "pitch": -0.008, "yaw": -1.028, "roll":  1.608 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z": -0.200, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
      { "X":  0.000, "Y":  0.000, "Z": -0.100, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 }
    ]
  },
  {
    "name": "Antenna",
    "is_abs": true,
    "origin": {"X": -0.249, "Y":  0.173, "Z":  0.968, "pitch":  0.936, "yaw": -0.900, "roll":  2.146 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 },
      { "X":  0.116, "Y":  0.046, "Z": -0.096, "pitch": -0.229, "yaw":  0.556, "roll": -0.163 }
    ]
  },
  {
    "name": "AntennaPull",
    "is_abs": true,
    "origin": {"X": -0.249, "Y":  0.173, "Z":  0.968, "pitch":  0.936, "yaw": -0.900, "roll":  2.146 },
    "sequence": [
      { "X":  0.000, "Y":  0.000, "Z":  0.000, "pitch":  0.000, "yaw":  0.000, "roll":  0.000 }
    ]
  }
]
	)data"};

struct SequenceEvent {
	std::chrono::milliseconds delay;
	RoboRioProtocol::Packet command;
	RoboRioProtocol::Packet reaction;
};

using namespace std::chrono_literals;

struct EventsForBellSequence {
	auto operator()() -> RoboRioProtocol::Packet {
		if (nextCommandIndex >= static_cast<decltype(nextCommandIndex)>(commands.size())) {
			std::this_thread::sleep_for(100ms);
			return RoboRioProtocol::Packet{KinovaFSM::Tick};
		}
		auto event = commands.at(nextCommandIndex);
		std::this_thread::sleep_for(event.delay);
		return event.command;
	}

	auto check(RoboRioProtocol::Packet const &reaction) -> bool {
		if (nextCommandIndex < static_cast<decltype(nextCommandIndex)>(commands.size())) {
			ASSERT_EQUAL(commands.at(nextCommandIndex).reaction, reaction);
			nextCommandIndex++;
		}
		if (reaction.command == KinovaFSM::SequenceDone) {
			endReached = true;
		}
		return true;
	}

	template<typename EventFake>
	void tickUntilEnd(CommandHandling<EventFake> &commandHandling) {
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
	auto objectives = std::istringstream{exampleData};
	std::shared_ptr<EventsForBellSequence> sequence = std::make_shared<EventsForBellSequence>();
	auto logger = Logging::makeLogger({"IntegrationSuite", {}, {}, {}});
	auto eventIo = std::make_unique<EventIOFake<EventsForBellSequence>>(sequence);
	auto jacoArm = std::make_unique<KinovaArm>(objectives, logger);
	CommandHandling<EventIOFake<EventsForBellSequence>> commandHandling{std::move(eventIo), std::move(jacoArm), logger};
	for (int i = 0; i < 8; i++) {
		commandHandling.process();
	}
	sequence->tickUntilEnd(commandHandling);
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s{};
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
