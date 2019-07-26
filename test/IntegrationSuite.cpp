#include "IntegrationSuite.h"
#include "cute.h"

void thisIsAIntegrationSuiteTest() {
	//Setup with EventIOFake and KinovaArmMock
	//Run for x Iterations?
}

cute::suite make_suite_IntegrationSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAIntegrationSuiteTest));
	return s;
}
