
#include "PositionHandlingSuite.h"
#include "cute.h"
#include "PositionHandling.h"

#include <array>
#include <sstream>

#include <string>


std::string const exampleData {
R"data(0 -400 500 1571 0 0 
-461 -267 525 -1989 -1418 2632 
120 -723 551 1491 -66 2 
0 0 0 0 0 0 
0 0 0 0 0 0 
-705 -28 755 -8 -1028 1608 
-249 173 968 936 -900 2146 
-249 173 968 936 -900 2146 

0 0 0 0 0 0 

0 0 -200 0 0 0 
0 0 -100 0 0 0 

-31 85 -175 -27 -28 20 
-40 80 -25 -9 -68 6 

0 0 0 0 0 0 
-3 8 0 -27 -8 713 
-33 15 -197 -60 192 598 
-63 0 -190 -59 69 -1543 
-70 83 -188 -53 55 -1555 
171 30 -319 -6 614 -106 
163 -191 -39 23 216 -125 

0 0 0 0 0 0 
-450 -80 -272 1006 -1298 902 
-576 -31 -375 1013 -1427 919 

0 0 -200 0 0 0 
0 0 -100 0 0 0 

0 0 0 0 0 0 
116 46 -96 -229 556 -163 

0 0 0 0 0 0 )data"
};

void testGetCoordinateForHomeObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, -0.4f, 0.5f, 1.571f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::Home);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForBellObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::Bell);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForHandleObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::Handle);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForOpenDoorObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::OpenDoor);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForPullDoorObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::PullDoor);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForPlaceCupObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::PlaceCup);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForAntennaObjective() {
	Kinova::Coordinates expectedCoordinates{-0.249f, 0.173f, 0.968f, 0.936f, -0.9f, 2.146f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::Antenna);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForAntennaPullObjective() {
	Kinova::Coordinates expectedCoordinates{-0.249f, 0.173f, 0.968f, 0.936f, -0.9f, 2.146f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::AntennaPull);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForNoObjectiveThrows() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT_THROWS(positionHandling.getCoordinates(Kinova::NoObjective), std::invalid_argument);
}

void testGetCoordinateThrowsForUnknownObjective() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT_THROWS(positionHandling.getCoordinates(static_cast<Kinova::Objective>(255)), std::invalid_argument);
}

void testGetCoordinateThrowsIfValidObjectiveHasNotBeenInitialized() {
	std::istringstream positionData{};
	PositionHandling const positionHandling{positionData};
	ASSERT_THROWS(positionHandling.getCoordinates(Kinova::Home), std::invalid_argument);
}

void testHasOriginForTargetWithoutOrigin() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT(!positionHandling.hasOrigin(Kinova::OpenDoor));
}

void testHasOriginForTargetWithOrigin() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT(positionHandling.hasOrigin(Kinova::Handle));
}

void testCompareContentOfSequence() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto location = positionHandling.getLocations();
	auto points = positionHandling.getPoints();
	auto sequences = positionHandling.getSequences();

	for (int objective = 0; objective < Kinova::NumberOfObjectives; objective++) {
		auto sequence = sequences[static_cast<Kinova::Objective>(objective + 1)];
		ASSERT_EQUAL(sequence.getOrigin(), Kinova::Coordinates{location[objective]});
		ASSERT_EQUAL(sequence.numberOfPoints(), points[objective].size());
		for (auto point : points[objective]) {
			ASSERT_EQUAL(sequence.getCurrentCoordinates(), Kinova::Coordinates{point});
			sequence.nextPoint();
		}
	}
}

cute::suite make_suite_PositionHandlingSuite() {
	cute::suite s { };
	s.push_back(CUTE(testGetCoordinateForHomeObjective));
	s.push_back(CUTE(testGetCoordinateForBellObjective));
	s.push_back(CUTE(testGetCoordinateForHandleObjective));
	s.push_back(CUTE(testGetCoordinateForOpenDoorObjective));
	s.push_back(CUTE(testGetCoordinateForPullDoorObjective));
	s.push_back(CUTE(testGetCoordinateForPlaceCupObjective));
	s.push_back(CUTE(testGetCoordinateForAntennaObjective));
	s.push_back(CUTE(testGetCoordinateForAntennaPullObjective));
	s.push_back(CUTE(testGetCoordinateForNoObjectiveThrows));
	s.push_back(CUTE(testGetCoordinateThrowsForUnknownObjective));
	s.push_back(CUTE(testGetCoordinateThrowsIfValidObjectiveHasNotBeenInitialized));
	s.push_back(CUTE(testCompareContentOfSequence));
	s.push_back(CUTE(testHasOriginForTargetWithoutOrigin));
	s.push_back(CUTE(testHasOriginForTargetWithOrigin));
	return s;
}
