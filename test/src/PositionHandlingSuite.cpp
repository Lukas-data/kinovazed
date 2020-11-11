
#include "PositionHandlingSuite.h"

#include "Logging.h"
#include "PositionHandling.h"
#include "TimesLiteral.h"

#include <cute/cute.h>

#include <sstream>
#include <string>
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


auto const logger = Logging::makeLogger({"PositionHandlingSuite", {}, {}, {}});

void testGetCoordinateForHomeObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, -0.4f, 0.5f, 1.571f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::Home);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForBellObjective() {
	Kinova::Coordinates expectedCoordinates{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::Bell);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForHandleObjective() {
	Kinova::Coordinates expectedCoordinates{0.10044f, -0.540117, 0.621585, 1.46395, -0.0940299, 0.0202154};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::Handle);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForOpenDoorObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::OpenDoor);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForPullDoorObjective() {
	Kinova::Coordinates expectedCoordinates{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::PullDoor);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForPlaceCupObjective() {
	Kinova::Coordinates expectedCoordinates{-0.533746, -0.0288264, 0.651697, -0.008, -1.028, 1.608};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::PlaceCup);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForAntennaObjective() {
	Kinova::Coordinates expectedCoordinates{-0.249f, 0.173f, 0.968f, 0.936f, -0.9f, 2.146f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::Antenna);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForAntennaPullObjective() {
	Kinova::Coordinates expectedCoordinates{-0.249f, 0.173f, 0.968f, 0.936f, -0.9f, 2.146f};
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::AntennaPull);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testGetCoordinateForNoObjectiveThrows() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT_THROWS(positionHandling.getCoordinates(Kinova::ObjectiveId::None), std::invalid_argument);
}

void testGetCoordinateThrowsForUnknownObjective() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT_THROWS(positionHandling.getCoordinates(static_cast<Kinova::ObjectiveId>(255)), std::out_of_range);
}

void testGetCoordinateThrowsIfValidObjectiveHasNotBeenInitialized() {
	std::istringstream positionData{};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT_THROWS(positionHandling.getCoordinates(Kinova::ObjectiveId::Home), std::out_of_range);
}

void testHasOriginForTargetWithoutOrigin() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT(positionHandling.getObjective(Kinova::ObjectiveId::OpenDoor).getOrigin().isZero());
}

void testHasOriginForTargetWithOrigin() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT(!positionHandling.getObjective(Kinova::ObjectiveId::Handle).getOrigin().isZero());
}

// void testHasOriginForInvalidTarget() {
// 	std::istringstream positionData{exampleData};
// 	ObjectiveManager const positionHandling{positionData, logger};
// 	ASSERT(!positionHandling.getObjective(static_cast<Kinova::ObjectiveId>(255)).hasOrigin());
// }

void testGetOriginForTargetWithOrigin() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	Kinova::Coordinates const expected{0.12f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f};
	ASSERT_EQUAL(expected, positionHandling.getObjective(Kinova::ObjectiveId::Handle).getOrigin().getCoordinates());
}

void testGetOriginForTargetWithoutOrigin() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	Kinova::Coordinates const expected{};
	ASSERT_EQUAL(expected, positionHandling.getObjective(Kinova::ObjectiveId::OpenDoor).getOrigin().getCoordinates());
}

void testGetOriginForInvalidTarget() {
	std::istringstream positionData{exampleData};
	ObjectiveManager const positionHandling{positionData, logger};
	ASSERT_THROWS(positionHandling.getObjective(static_cast<Kinova::ObjectiveId>(255)).getOrigin(), std::out_of_range);
}

void assertFullSequence(ObjectiveManager &positionHandling,
                        Kinova::ObjectiveId objective,
                        std::vector<Kinova::Coordinates> expectedPoints) {
	positionHandling.resetSequence(objective);
	for (auto const &point : expectedPoints) {
		auto currentCoordinates = positionHandling.getCoordinates(objective);
		ASSERT_EQUAL(point, currentCoordinates);
		positionHandling.incrementSequence(objective);
	}
	ASSERT(positionHandling.resetOriginAtEnd(objective));
}

void testSavePointForBell() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const pointToAdd{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f};

	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::ObjectiveId::Bell));
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}


void testSavePointForBellAtEndOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	2_times([&] { positionHandling.incrementSequence(Kinova::ObjectiveId::Bell); });
	Kinova::Coordinates const pointToAdd{-0.420609, -0.317719, 0.573951, -1.84516, -1.45474, 2.84915};
	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::ObjectiveId::Bell));
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f},
	                    {-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f}});
}


void testSavePointForBellAfterEndOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	3_times([&] { positionHandling.incrementSequence(Kinova::ObjectiveId::Bell); });
	Kinova::Coordinates const pointToAdd{0.04f, 0.05f, -0.05f, -0.033f, 0.025f, 0.075f};
	ASSERT(!positionHandling.savePoint(pointToAdd, Kinova::ObjectiveId::Bell));
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}


void testSavePointForBellBeforeSequence() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	positionHandling.decrementSequence(Kinova::ObjectiveId::Bell);
	Kinova::Coordinates const pointToAdd{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f};
	ASSERT(positionHandling.savePoint(pointToAdd, Kinova::ObjectiveId::Bell));
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.420609f, -0.317719f, 0.573951f, -1.84516f, -1.45474f, 2.84915f},
	                    {-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}


void testSaveOriginSetsOrigin() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	positionHandling.saveOrigin(newOrigin, Kinova::ObjectiveId::Handle);
	ASSERT_EQUAL(newOrigin, positionHandling.getObjective(Kinova::ObjectiveId::Handle).getOrigin().getCoordinates());
}

void testSaveOriginUpdatesTranformationMatrix() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	Kinova::Coordinates expectedCoordinates{-0.122555f, -0.0473307f, 0.311046f, -1.51853f, -0.187887f, 0.0136127f};
	positionHandling.saveOrigin(newOrigin, Kinova::ObjectiveId::Handle);
	auto coordinates = positionHandling.getCoordinates(Kinova::ObjectiveId::Handle);
	ASSERT_EQUAL(expectedCoordinates, coordinates);
}

void testSetZeroObjectiveForZeroObjective() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const newOrigin{-0.12f, 0.123f, 0.41f, -1.491f, -0.16f, -0.002f};
	positionHandling.setZeroObjective(newOrigin, Kinova::ObjectiveId::OpenDoor);
	ASSERT_EQUAL(newOrigin, positionHandling.getObjective(Kinova::ObjectiveId::OpenDoor).getOrigin().getCoordinates());
}

void testSetZeroObjectiveForNonZeroObjective() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	Kinova::Coordinates const expected{0.12f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f};
	positionHandling.setZeroObjective(newOrigin, Kinova::ObjectiveId::Handle);
	ASSERT_EQUAL(expected, positionHandling.getObjective(Kinova::ObjectiveId::Handle).getOrigin().getCoordinates());
}

void testResetOriginAtEndResetsOriginAtEndOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	auto const originalOrigin = positionHandling.getObjective(Kinova::ObjectiveId::PullDoor).getOrigin();
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	positionHandling.setZeroObjective(newOrigin, Kinova::ObjectiveId::PullDoor);
	3_times([&] { positionHandling.incrementSequence(Kinova::ObjectiveId::PullDoor); });
	positionHandling.resetOriginAtEnd(Kinova::ObjectiveId::PullDoor);
	ASSERT_EQUAL(originalOrigin, positionHandling.getObjective(Kinova::ObjectiveId::PullDoor).getOrigin());
}

void testResetOriginAtEndDoesNotResetOriginAtBeginningOfSequence() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	Kinova::Coordinates const newOrigin{0.1f, 0.1f, 0.1f, 1.0f, 0.1f, 0.001f};
	positionHandling.setZeroObjective(newOrigin, Kinova::ObjectiveId::PullDoor);
	ASSERT(!positionHandling.resetOriginAtEnd(Kinova::ObjectiveId::PullDoor));
	ASSERT_EQUAL(newOrigin, positionHandling.getObjective(Kinova::ObjectiveId::PullDoor).getOrigin().getCoordinates());
}

void testDeletePointAtBeginningOfSequence() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	positionHandling.deletePoint(Kinova::ObjectiveId::Bell);
	assertFullSequence(
	    positionHandling, Kinova::ObjectiveId::Bell, {{-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}

void testDeletePointAtEndOfSequence() {
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	positionHandling.incrementSequence(Kinova::ObjectiveId::Bell);
	positionHandling.deletePoint(Kinova::ObjectiveId::Bell);
	assertFullSequence(
	    positionHandling, Kinova::ObjectiveId::Bell, {{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f}});
}

void testDeletePointBeyondEndOfSequenceDoesNotDeletePoint() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	2_times([&] { positionHandling.incrementSequence(Kinova::ObjectiveId::Bell); });
	positionHandling.deletePoint(Kinova::ObjectiveId::Bell);
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}

void testDeletePointBeforeBeginningEndOfSequenceDoesNotDeletePoint() {
	using namespace TimesLiteral;
	std::istringstream positionData{exampleData};
	ObjectiveManager positionHandling{positionData, logger};
	positionHandling.decrementSequence(Kinova::ObjectiveId::Bell);
	positionHandling.deletePoint(Kinova::ObjectiveId::Bell);
	assertFullSequence(positionHandling,
	                   Kinova::ObjectiveId::Bell,
	                   {{-0.26333f, -0.294817f, 0.537362f, -1.989f, -1.418f, 2.632f},
	                    {-0.362165f, -0.280909f, 0.531181f, -1.989f, -1.418f, 2.632f}});
}

// void testCompareContentOfSequence() {
//	std::istringstream positionData{exampleData};
//	ObjectiveManager const positionHandling{positionData};
//	auto location = positionHandling.getLocations();
//	auto points = positionHandling.getPoints();
//	auto sequences = positionHandling.getSequences();
//
//	for (int objective = 0; objective < Kinova::NumberOfObjectives; objective++) {
//		auto sequence = sequences[static_cast<Kinova::Objective>(objective + 1)];
//		ASSERT_EQUAL(sequence.getOrigin(), Kinova::Coordinates{location[objective]});
//		ASSERT_EQUAL(sequence.length(), points[objective].size());
//		for (auto point : points[objective]) {
//			ASSERT_EQUAL(sequence.currentPoint(), Kinova::Coordinates{point});
//			sequence.advance();
//		}
//	}
//}

cute::suite make_suite_PositionHandlingSuite() {
	cute::suite s{};
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
	// s.push_back(CUTE(testHasOriginForInvalidTarget));
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
	s.push_back(CUTE(testDeletePointAtBeginningOfSequence));
	s.push_back(CUTE(testDeletePointAtEndOfSequence));
	s.push_back(CUTE(testDeletePointBeyondEndOfSequenceDoesNotDeletePoint));
	s.push_back(CUTE(testDeletePointBeforeBeginningEndOfSequenceDoesNotDeletePoint));

	//	s.push_back(CUTE(testCompareContentOfSequence));
	return s;
}
