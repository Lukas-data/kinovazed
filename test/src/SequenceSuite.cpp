#include "SequenceSuite.h"

#include "Logging.h"
#include "MatrixHelper.h"
#include "Sequence.h"

#include <cute/cute.h>

using Kinova::Coordinates;
using Kinova::Origin;
using Kinova::Sequence;

Origin const testOrigin{{0.120f, -0.723f, 0.551f, 1.491f, -0.066f, 0.002f}};
Coordinates const firstPoint{-0.031f, 0.085f, -0.175f, -0.027f, -0.028f, 0.020f};
Coordinates const secondPoint{-0.040f, 0.080f, -0.025f, -0.009f, -0.068f, 0.006f};

auto createTestSequence() {
	auto logger = Logging::makeLogger({"SequenceSuite", {}, {}, {}});
	return Sequence{{firstPoint, secondPoint}, logger};
}

void testGetInitialPoint() {
	auto const sequence = createTestSequence();
	ASSERT_EQUAL(firstPoint, sequence.currentPoint());
}

void testNextPoint() {
	auto sequence = createTestSequence();
	sequence.advance();
	ASSERT_EQUAL(secondPoint, sequence.currentPoint());
}

void testReset() {
	auto sequence = createTestSequence();
	sequence.advance();
	sequence.reset();
	ASSERT_EQUAL(firstPoint, sequence.currentPoint());
}

void testPreviousPoint() {
	auto sequence = createTestSequence();
	sequence.advance();
	sequence.stepBack();
	ASSERT_EQUAL(firstPoint, sequence.currentPoint());
}

void testEndReached() {
	auto sequence = createTestSequence();
	sequence.advance();
	sequence.advance();
	ASSERT(sequence.endReached());
}

void testNumberOfPoints() {
	auto const sequence = createTestSequence();
	ASSERT_EQUAL(2, sequence.length());
}

void testGetCurrentCoordinatesAtEndThrows() {
	auto sequence = createTestSequence();
	sequence.advance();
	sequence.advance();
	ASSERT_THROWS(sequence.currentPoint(), std::logic_error);
}

void testNextPointAtEndThrows() {
	auto sequence = createTestSequence();
	sequence.advance();
	sequence.advance();
	ASSERT_THROWS(sequence.advance(), std::logic_error);
}

void testPreviousPointAtBeginningThrows() {
	auto sequence = createTestSequence();
	ASSERT_THROWS(sequence.stepBack(), std::logic_error);
}

void testOriginTransformationMatrix() {
	Kinova::f2d_vec_t const expectedMatrix{{0.997821f, -0.00199564f, -0.0659521f, 0.12},
	                                       {-0.0655827f, 0.0798429f, -0.994648f, -0.723},
	                                       {0.00725077f, 0.996805f, 0.0795381, 0.551f},
	                                       {0.0f, 0.0f, 0.0f, 1.0f}};
	auto const transformationMatrix = testOrigin.getTransformationMatrix();
	assertMatrixEqual(expectedMatrix, transformationMatrix);
}

void testOriginInvertedTransformationMatrix() {
	Kinova::f2d_vec_t const expectedMatrix{{0.997821f, -0.0655827f, 0.00725077f, -0.17115},
	                                       {-0.00199564f, 0.0798429f, 0.996805f, -0.491274},
	                                       {-0.0659521f, -0.994648f, 0.0795381, -0.755041},
	                                       {0.0f, 0.0f, 0.0f, 1.0f}};
	auto const invertedTransformationMatrix = testOrigin.getInvertedTransformationMatrix();
	assertMatrixEqual(expectedMatrix, invertedTransformationMatrix);
}

void testGetTransformedCoordinates() {
	auto const sequence = createTestSequence();
	Coordinates const coordData{0.10044f, -0.540117f, 0.621585f, 1.46395f, -0.0940299f, 0.0202154f};
	ASSERT_EQUAL(coordData, sequence.currentPoint(testOrigin));
}


cute::suite make_suite_SequenceSuite() {
	cute::suite s{};
	s.push_back(CUTE(testGetInitialPoint));
	s.push_back(CUTE(testNextPoint));
	s.push_back(CUTE(testReset));
	s.push_back(CUTE(testPreviousPoint));
	s.push_back(CUTE(testEndReached));
	s.push_back(CUTE(testNumberOfPoints));
	s.push_back(CUTE(testGetCurrentCoordinatesAtEndThrows));
	//	s.push_back(CUTE(testNextPointAtEndThrows)); //Test disabled as teaching might require sequence number to be -1
	//	s.push_back(CUTE(testPreviousPointAtBeginningThrows)); //Test disabled as teaching might require sequence number
	// to be higher than number of points
	s.push_back(CUTE(testOriginTransformationMatrix));
	s.push_back(CUTE(testOriginInvertedTransformationMatrix));
	s.push_back(CUTE(testGetTransformedCoordinates));
	return s;
}
