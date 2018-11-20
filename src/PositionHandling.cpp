#include <stdio.h>
#include <stdexcept>
#include "PositionHandling.h"


//declare Default Positions
KinovaPts::posCoordinates PositionHandling::Positions[NUMBER_OF_POINTS][NUMBER_OF_SUBPOINTS] = {
  { //HOME
  { -0.080854, -0.394615, 0.488780, 1.551021, 0.190894, -0.080310}, //HOME POSITIONS
  //{ 0, 0, 0, 0, 0, 0}
  },
  { //BELL (TODO: REAL VALUES)
  { 0.200632, -0.425328, 0.459094, 1.565757, 0.825559, -0.063724}, //BELL OFFSET POSITION
  { 0.200761, -0.518639, 0.456269, 1.568696, 0.751777, -0.059473}  //BELL POSITION  
  }
};

PositionHandling::~PositionHandling() {
  
}


/*void PositionHandling::error(const char *msg)
{
  //perror(msg);
  //throw std::runtime_error(msg);
}*/


//Writes Requestet Coordinates of Position by pos-number and its Subpositions by Sequence-number.
//Returns 0 if sequence has ended.
bool PositionHandling::getCoordinates(float* coordinates, KinovaPts::Points targetPoint) {
  
  //Check SequenceNumber
  if (SequenceCounter >= NUMBER_OF_SUBPOINTS) {
  SequenceCounter = 0;
  return 0;
  }
 
  //static float coordinates[6];
  coordinates[0] = Positions[targetPoint][SequenceCounter].x;
  coordinates[1] = Positions[targetPoint][SequenceCounter].y;
  coordinates[2] = Positions[targetPoint][SequenceCounter].z;
  coordinates[3] = Positions[targetPoint][SequenceCounter].pitch;
  coordinates[4] = Positions[targetPoint][SequenceCounter].yaw;
  coordinates[5] = Positions[targetPoint][SequenceCounter].roll;

  if (coordinates[0] == 0 && coordinates[1] == 0 && coordinates[2] == 0 &&
      coordinates[3] == 0 && coordinates[4] == 0 && coordinates[5] == 0) {
    SequenceCounter = 0;
    return 0;
  }
}

void PositionHandling::countSequence() {
  ++SequenceCounter;
}
