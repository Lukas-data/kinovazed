#include <stdio.h>
#include <stdexcept>
#include "PositionHandling.h"




using namespace std;

//declare Default Positions
posCoordinates PositionHandling::Positions[NUMBER_OF_POINTS][NUMBER_OF_SUBPOINTS] = {
  { //HOME
  { -0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310}, //HOME POSITIONS
  //{ 0, 0, 0, 0, 0, 0}
  },
  { //BELL (TODO: REAL VALUES)
  { -0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310}, //BELL OFFSET POSITION
  { -0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310}  //BELL POSITION  
  }
};





PositionHandling::PositionHandling() {
}

PositionHandling::~PositionHandling() {
  
}




/*void PositionHandling::error(const char *msg)
{
  //perror(msg);
  //throw std::runtime_error(msg);
}*/


//Writes Requestet Coordinates of Position by pos-number and its Subpositions by Sequence-number.
//Returns 1 if sequence has ended.
bool PositionHandling::getCoordinates(float* coordinates, int pos, int sequence) {
  
  //Check SequenceNumber
  if (sequence >= NUMBER_OF_SUBPOINTS) {
  printf("End Of Sequence");
  return 1;
  } 
  //Chcek Pos-Number
  if (pos >= NUMBER_OF_POINTS) {
  throw std::runtime_error("PositionHandling.cpp, getCoordinates(), Check for int pos failed: No such point stored.");
  return 0;
  } 

  //static float coordinates[6];

  coordinates[0] = Positions[pos][0].x;
  coordinates[1] = Positions[pos][0].y;
  coordinates[2] = Positions[pos][0].z;
  coordinates[3] = Positions[pos][0].pitch;
  coordinates[4] = Positions[pos][0].yaw;
  coordinates[5] = Positions[pos][0].roll;

}
