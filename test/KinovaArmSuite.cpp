#include "KinovaArmSuite.h"
#include "KinovaArm.h"
#include "cute.h"

void thisIsAKinovaArmSuiteTest() {
	KinovaArm arm{};
}

cute::suite make_suite_KinovaArmSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAKinovaArmSuiteTest));
	return s;
}
