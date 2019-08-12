
#include "PositionHandlingSuite.h"
#include "cute.h"
#include "PositionHandling.h"
#include "TimesLiteral.h"

#include <sstream>
#include <string>
#include <vector>



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
	Kinova::Coordinates expectedCoordinates{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f};
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	auto coordinates = positionHandling.getCoordinates(Kinova::Bell);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForHandleObjective() {
	Kinova::Coordinates expectedCoordinates{0.10044f, -0.540117, 0.621585, 1.46395, -0.0940299, 0.0202154};
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
	Kinova::Coordinates expectedCoordinates{-0.533746, -0.0288264, 0.651697, -0.008, -1.028, 1.608};
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

void testHasOriginForInvalidTarget() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT(!positionHandling.hasOrigin(static_cast<Kinova::Objective>(255)));
}

void testGetOriginForTargetWithOrigin() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	Kinova::Coordinates const expected{0.12f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f };
	ASSERT_EQUAL(expected, positionHandling.getOrigin(Kinova::Handle));
}

void testGetOriginForTargetWithoutOrigin() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	Kinova::Coordinates const expected{};
	ASSERT_EQUAL(expected, positionHandling.getOrigin(Kinova::OpenDoor));
}

void testGetOriginForInvalidTarget() {
	std::istringstream positionData{exampleData};
	PositionHandling const positionHandling{positionData};
	ASSERT_THROWS(positionHandling.getOrigin(static_cast<Kinova::Objective>(255)), std::out_of_range);
}

void assertFullSequence(PositionHandling & positionHandling, Kinova::Objective objective, std::vector<Kinova::Coordinates> expectedPoints) {
	positionHandling.resetSequence();
	for (auto const & point : expectedPoints) {
		auto currentCoordinates = positionHandling.getCoordinates(objective);
		ASSERT_EQUAL(point, currentCoordinates);
		positionHandling.incrementSequence();
	}
	ASSERT(positionHandling.resetOriginAtEnd(objective));
}

void testSavePointForBell() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const pointToAdd{0.04f, 0.05f, -0.05f, -0.033f, 0.025f, 0.075f};
	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::Bell));
	assertFullSequence(positionHandling, Kinova::Bell, {
			{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f},
			{-0.362165f, -0.280909f, 0.531181f, -1.989f,   -1.418f,   2.632f}
		}
	);
}


void testSavePointForBellAtEndOfSequence() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	positionHandling.incrementSequence();
	positionHandling.incrementSequence();
	Kinova::Coordinates const pointToAdd{0.04f, 0.05f, -0.05f, -0.033f, 0.025f, 0.075f};
	Kinova::Coordinates const expected{-0.420609, -0.317719, 0.573951, -1.84516, -1.45474, 2.84915};
	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::Bell));
	assertFullSequence(positionHandling, Kinova::Bell, {
			{-0.26333f,  -0.294817f, 0.537362f, -1.989f,   -1.418f,   2.632f},
			{-0.362165f, -0.280909f, 0.531181f, -1.989f,   -1.418f,   2.632f},
			{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f}
		}
	);
}


void testSavePointForBellAfterEndOfSequence() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	positionHandling.incrementSequence();
	positionHandling.incrementSequence();
	positionHandling.incrementSequence();
	Kinova::Coordinates const pointToAdd{0.04f, 0.05f, -0.05f, -0.033f, 0.025f, 0.075f};
	ASSERT(!positionHandling.savePoint(pointToAdd, Kinova::Bell));
	assertFullSequence(positionHandling, Kinova::Bell, {
			{-0.26333f,  -0.294817f, 0.537362f, -1.989f,  -1.418f,  2.632f},
			{-0.362165f, -0.280909f, 0.531181f, -1.989f,   -1.418f, 2.632f}
		}
	);
}


void testSavePointForBellBeforeSequence() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	positionHandling.decrementSequence();
	Kinova::Coordinates const pointToAdd{0.04f, 0.05f, -0.05f, -0.033f, 0.025f, 0.075f};
	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::Bell));
	assertFullSequence(positionHandling, Kinova::Bell, {
			{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f},
			{-0.26333f,  -0.294817f, 0.537362f, -1.989f,   -1.418f,   2.632f},
			{-0.362165f, -0.280909f, 0.531181f, -1.989f,   -1.418f,   2.632f}
		}
	);
}


void testSaveOriginSetsOrigin() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	positionHandling.saveOrigin(newOrigin, Kinova::Handle);
	ASSERT_EQUAL(newOrigin, positionHandling.getOrigin(Kinova::Handle));
}

void testSaveOriginUpdatesTranformationMatrix() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	Kinova::Coordinates expectedCoordinates{-0.122555f, -0.0473307f, 0.311046f, -1.51853f, -0.187887f, 0.0136127f};
	positionHandling.saveOrigin(newOrigin, Kinova::Handle);
	auto coordinates = positionHandling.getCoordinates(Kinova::Handle);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testSetZeroObjectiveForZeroObjective() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	positionHandling.setZeroObjective(newOrigin, Kinova::OpenDoor);
	ASSERT_EQUAL(newOrigin, positionHandling.getOrigin(Kinova::OpenDoor));
}

void testSetZeroObjectiveForNonZeroObjective() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	Kinova::Coordinates const expected{0.12f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f};
	positionHandling.setZeroObjective(newOrigin, Kinova::Handle);
	ASSERT_EQUAL(expected, positionHandling.getOrigin(Kinova::Handle));
}

void testResetOriginAtEndResetsOriginAtEndOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	auto const originalOrigin = positionHandling.getOrigin(Kinova::PullDoor);
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	positionHandling.setZeroObjective(newOrigin, Kinova::PullDoor);
	3_times([&]{positionHandling.incrementSequence();});
	positionHandling.resetOriginAtEnd(Kinova::PullDoor);
	ASSERT_EQUAL(originalOrigin, positionHandling.getOrigin(Kinova::PullDoor));
}

void testResetOriginAtEndDoesNotResetOriginAtBeginningOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	positionHandling.setZeroObjective(newOrigin, Kinova::PullDoor);
	ASSERT(!positionHandling.resetOriginAtEnd(Kinova::PullDoor));
	ASSERT_EQUAL(newOrigin, positionHandling.getOrigin(Kinova::PullDoor));
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
	s.push_back(CUTE(testHasOriginForTargetWithoutOrigin));
	s.push_back(CUTE(testHasOriginForTargetWithOrigin));
	s.push_back(CUTE(testHasOriginForInvalidTarget));
	s.push_back(CUTE(testGetOriginForTargetWithOrigin));
	s.push_back(CUTE(testGetOriginForTargetWithoutOrigin));
	s.push_back(CUTE(testGetOriginForInvalidTarget));
	s.push_back(CUTE(testSavePointForBell));
	s.push_back(CUTE(testSavePointForBellAtEndOfSequence));
	s.push_back(CUTE(testSavePointForBellAfterEndOfSequence));
	s.push_back(CUTE(testSavePointForBellBeforeSequence));
	s.push_back(CUTE(testSaveOriginSetsOrigin));
	s.push_back(CUTE(testSaveOriginUpdatesTranformationMatrix));
	s.push_back(CUTE(testSetZeroObjectiveForZeroObjective));
	s.push_back(CUTE(testSetZeroObjectiveForNonZeroObjective));
	s.push_back(CUTE(testResetOriginAtEndResetsOriginAtEndOfSequence));
	s.push_back(CUTE(testResetOriginAtEndDoesNotResetOriginAtBeginningOfSequence));

	s.push_back(CUTE(testCompareContentOfSequence));
	return s;
}
