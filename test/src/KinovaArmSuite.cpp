#include "KinovaArmSuite.h"

#include "KinovaArm.h"
#include "Logging.h"

#include <cute/cute.h>

auto logger = Logging::makeLogger({"KinovaArmSuite", {}, {}, {}});

void thisIsAKinovaArmSuiteTest() {
	KinovaArm arm{logger};
}

cute::suite make_suite_KinovaArmSuite() {
	cute::suite s{};
	s.push_back(CUTE(thisIsAKinovaArmSuiteTest));
	return s;
}
