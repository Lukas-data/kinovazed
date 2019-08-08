#include "PositionHandlingSuite.h"
#include "cute.h"
#include "PositionHandling.h"

#include <string>


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

void thisIsAPositionHandlingSuiteTest() {
	std::istringstream positionData{exampleData};
	PositionHandling positionHandling{positionData};
}

cute::suite make_suite_PositionHandlingSuite() {
	cute::suite s { };
	s.push_back(CUTE(thisIsAPositionHandlingSuiteTest));
	return s;
}
