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

cute::suite make_suite_SequenceSuite() {
	cute::suite s{};
	s.push_back(CUTE(testGetOrigin));
	s.push_back(CUTE(testGetInitialPoint));
	s.push_back(CUTE(testNextPoint));
	return s;
}
