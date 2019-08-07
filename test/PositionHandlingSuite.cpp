#include "PositionHandlingSuite.h"
#include "cute.h"
#include "PositionHandling.h"


void thisIsAPositionHandlingSuiteTest() {
	PositionHandling positionHandling{};
}

cute::suite make_suite_PositionHandlingSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAPositionHandlingSuiteTest));
	return s;
}
