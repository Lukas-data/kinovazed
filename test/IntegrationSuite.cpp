#include "IntegrationSuite.h"
#include "cute.h"
#include "CommandHandling.h"
#include "Event.h"
#include "EventIOFake.h"
#include "RoboRioProtocol.h"

#include <thread>
#include <vector>

struct EventsForBellSequence {
	auto operator()() -> RoboRioProtocol::Packet {
		if (nextCommandIndex >= commands.size()) {
			FAILM("End of command sequence reached");
		}
		return commands.at(nextCommandIndex++);
	}
private:
	int nextCommandIndex{0};
	std::vector<RoboRioProtocol::Packet> commands{
		{KinovaFSM::Initialize},
		{KinovaFSM::Tick},
		{KinovaFSM::Initialized},
		{KinovaFSM::Tick},
		{KinovaFSM::Unfold},
		{KinovaFSM::Tick},
		{KinovaFSM::Unfolded},
		{KinovaFSM::GoToPosition, 2},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},
		{KinovaFSM::Tick},

	};
};

void thisIsAIntegrationSuiteTest() {
	using namespace std::chrono_literals;
	CommandHandling<EventIOFake<EventsForBellSequence>> commandHandling{};
	for (int i = 0; i < 16; i++) {
		commandHandling.process();
		std::this_thread::sleep_for(1s);
	}
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
