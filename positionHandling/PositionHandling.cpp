#include "PositionHandling.h"


#include <stdio.h>


using namespace std;

//declare Default Positions
posCoordinates PositionHandling::Positions[NUMBER_OF_POINTS] = {
{ -0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310}
};



PositionHandling::PositionHandling() {

}

PositionHandling::~PositionHandling() {
  
}


void PositionHandling::error(const char *msg)
{
  //perror(msg);
  //throw std::runtime_error(msg);
}



void PositionHandling::getCoordinates(float* coordinates, int pos) {

  //static float coordinates[6];

  coordinates[0] = Positions[pos].x;
  coordinates[1] = Positions[pos].y;
  coordinates[2] = Positions[pos].z;
  coordinates[3] = Positions[pos].pitch;
  coordinates[4] = Positions[pos].yaw;
  coordinates[5] = Positions[pos].roll;

}
