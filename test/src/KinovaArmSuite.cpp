#include "KinovaArmSuite.h"

#include "KinovaArm.h"
#include "Logging.h"

#include <cute/cute.h>

#include <sstream>

auto logger = Logging::makeLogger({"KinovaArmSuite", {}, {}, {}});

void thisIsAKinovaArmSuiteTest() {
	auto objectives = std::istringstream{};
	KinovaArm arm{objectives, logger};
}

cute::suite make_suite_KinovaArmSuite() {
	cute::suite s{};
	s.push_back(CUTE(thisIsAKinovaArmSuiteTest));
	return s;
}
