#include "SequenceSuite.h"
#include "Sequence.h"
#include "cute.h"

using Kinova::Coordinates;
using Kinova::Sequence;

Coordinates const testOrigin{0.120f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f};
Coordinates const firstPoint{-0.031f, 0.085f, -0.175f, -0.027f, -0.028f, 0.020f};
Coordinates const secondPoint{-0.040f, 0.080f, -0.025f, -0.009f, -0.068f, 0.006f};

auto createTestSequence() {
	return Sequence{testOrigin, {firstPoint, secondPoint}};
}

void testGetOrigin() {
	auto const sequence = createTestSequence();
	ASSERT_EQUAL(testOrigin, sequence.getOrigin());
}

void testGetInitialPoint() {
	auto const sequence = createTestSequence();
	ASSERT_EQUAL(firstPoint, sequence.getCurrentCoordinates());
}

void testNextPoint() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	ASSERT_EQUAL(secondPoint, sequence.getCurrentCoordinates());
}

void testReset() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	sequence.reset();
	ASSERT_EQUAL(firstPoint, sequence.getCurrentCoordinates());
}

void testPreviousPoint() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	sequence.previousPoint();
	ASSERT_EQUAL(firstPoint, sequence.getCurrentCoordinates());
}

void testEndReached() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	sequence.nextPoint();
	ASSERT(sequence.endReached());
}

void testGetCurrentCoordinatesAtEndThrows() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	sequence.nextPoint();
	ASSERT_THROWS(sequence.getCurrentCoordinates(), std::logic_error);
}

void testNextPointAtEndThrows() {
	auto sequence = createTestSequence();
	sequence.nextPoint();
	sequence.nextPoint();
	ASSERT_THROWS(sequence.nextPoint(), std::logic_error);
}

void testPreviousPointAtBeginningThrows() {
	auto sequence = createTestSequence();
	ASSERT_THROWS(sequence.previousPoint(), std::logic_error);
}

cute::suite make_suite_SequenceSuite() {
	cute::suite s{};
	s.push_back(CUTE(testGetOrigin));
	s.push_back(CUTE(testGetInitialPoint));
	s.push_back(CUTE(testNextPoint));
	s.push_back(CUTE(testReset));
	s.push_back(CUTE(testPreviousPoint));
	s.push_back(CUTE(testEndReached));
	s.push_back(CUTE(testGetCurrentCoordinatesAtEndThrows));
	s.push_back(CUTE(testNextPointAtEndThrows));
	s.push_back(CUTE(testPreviousPointAtBeginningThrows));
	return s;
}
