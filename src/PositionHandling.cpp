//#include <stdio.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
//#include <jansson.h>
//#include "../thirdparty/jansson-2.12/src/jansson.h"
#include "PositionHandling.h"


//declare Default Positions
KinovaPts::posCoordinates PositionHandling::Locations[KinovaPts::NumberOfObjectives][NUMBER_OF_SUBPOINTS] = {
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
  writeToFile();
}


/*void PositionHandling::error(const char *msg)
{
  //perror(msg);
  //throw std::runtime_error(msg);
}*/


//Writes Requestet Coordinates of Position by pos-number and its Subpositions by Sequence-number.
//Returns 0 if sequence has ended.
bool PositionHandling::getCoordinates(float* coordinates, KinovaPts::Objective targetObjective) {
  
  //Check SequenceNumber
  if (SequenceCounter >= NUMBER_OF_SUBPOINTS) { return 0;}
 
  //static float coordinates[6];
  coordinates[0] = Locations[targetObjective-1][SequenceCounter].x;
  coordinates[1] = Locations[targetObjective-1][SequenceCounter].y;
  coordinates[2] = Locations[targetObjective-1][SequenceCounter].z;
  coordinates[3] = Locations[targetObjective-1][SequenceCounter].pitch;
  coordinates[4] = Locations[targetObjective-1][SequenceCounter].yaw;
  coordinates[5] = Locations[targetObjective-1][SequenceCounter].roll;

  if (coordinates[0] == 0 && coordinates[1] == 0 && coordinates[2] == 0 &&
      coordinates[3] == 0 && coordinates[4] == 0 && coordinates[5] == 0) {
    return 0;
  }
}

void PositionHandling::countSequence() {
  ++SequenceCounter;
}
void PositionHandling::resetSequence() {
  SequenceCounter = 0;
}

/*Saves coordinates to current Sequence Point in object.*/
void PositionHandling::savePoint(float coordinates[6], KinovaPts::Objective targetObjective) {
  Locations[targetObjective-1][SequenceCounter].x = coordinates[0];
  Locations[targetObjective-1][SequenceCounter].y = coordinates[1];
  Locations[targetObjective-1][SequenceCounter].z = coordinates[2];
  Locations[targetObjective-1][SequenceCounter].pitch = coordinates[3];
  Locations[targetObjective-1][SequenceCounter].yaw = coordinates[4];
  Locations[targetObjective-1][SequenceCounter].roll = coordinates[5];
}


/*Reads Location and Points Vectors from SaveFile*/
void PositionHandling::readFromFile() {
  std::string line;
  std::ifstream infile ("testPosition.dat");
  
  //Reads Objectives from Files
  for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
    if (std::getline(infile,line)) {     
      std::istringstream iss(line);
      int n;
      std::vector<int> vec_int;
      vec_int.reserve(6);
      while (iss >> n) {
        vec_int.push_back(n);
      }
      if (vec_int.size() == 0) {
        std::cout << "No Location. ERROR: File-Missmatch.";
        break;
      }
      for (int j = 0; j < 6; j++) {
        Location[i][j] = (float)vec_int[j]/1000;
      } 
    }
  }
  std::getline(infile,line);

  //Reads Points from Files
  int location = 0;
  int sequence = 0;
  bool PrePoint = true;
  cord_vec_t::iterator it = Points[location].begin();
  while (std::getline(infile,line) && location < KinovaPts::NumberOfObjectives) { 
    std::istringstream iss(line);
    int n;
    std::vector<int> vec_int;
    vec_int.reserve(6);
    while (iss >> n) {
      vec_int.push_back(n);
    }
    if (vec_int.size() == 0) {
      location++;
      sequence = 0;
      it = Points[location].begin();
      PrePoint = true;
    }
    else {
      std::vector<float> vec_float(6);
      bool isZero = true;
      for (int i = 0; i < 6; i++) {
        if (vec_int[i] != 0) {
          isZero = false;
        }
        vec_float[i] = (float)vec_int[i]/1000;
      }
      if (isZero) {
        std::cout << "isZero: " << isZero << "\n";
        PrePoint = false;
      }
      else if (PrePoint) {
        std::cout << "insert\n";
        Points[location].insert(it+sequence, vec_float);
      }
      else {
        std::cout << "PushBack\n";
        Points[location].push_back(vec_float);
      }
      sequence++;
    }
  }
  std::cout << "File Loaded\n";
}


/*Writes Location and Points Vectors to SaveFile*/
void PositionHandling::writeToFile() {
  std::cout << "Save File\n";
  std::ofstream saveFile ("Positions.dat");
  if ( saveFile.is_open() ) {
    //Write Locations
    for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
      for (int j = 0; j < 6; j++) {
        saveFile << (int)(Location[i][j]*1000) << " ";
      }
      saveFile << "\n";
    }
    saveFile << "\n";
    //Write Sequences
    for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
      for (int j = 0; j < Points[i].size(); j++) {
        for (int k = 0; k < 6; k++) {
          saveFile << (int)(Points[i][j][k]*1000) << " ";
        }
        saveFile << "\n";
      }
      saveFile << "\n";
    }
  }
  else std::cout << "Unable to open file\n";
}

int PositionHandling::getSequence() {
  return SequenceCounter;
}
