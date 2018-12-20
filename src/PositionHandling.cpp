#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <math.h>
#include "PositionHandling.h"


#define FILEPATH "/home/kinova/Documents/kinovaZED/CybathlonObjectives.dat"


PositionHandling::~PositionHandling() {
  writeToFile();
}


void PositionHandling::init() {
  readFromFile();
  calcTransMat();
}


//Writes Requestet targetCoordinates[6] of Objective by pos-number and its Subpositions by Sequence-number. Takes currentPosition[6] if Objective is 0.
//Returns 0 if sequence has ended.
bool PositionHandling::getCoordinates(float* targetCoordinates, KinovaPts::Objective targetObjective, float* currentCoordinates) {
  
  //Check if ZeroObjective.  Inserts currentCoordinates and recalcs TransMat at beginning of Sequence.
  if (std::find(ZeroObjectives.begin(), ZeroObjectives.end(), targetObjective-1) != ZeroObjectives.end() && SequenceCounter == 0) {
    for (int i = 0; i < 6; i++) {
      Location[targetObjective-1][i] = currentCoordinates[i];
    }
    calcTransMat();
  }

  //Check SequenceNumber. Returns 0 and resets ZeroObjective if Sequence is over.
  if (SequenceCounter >= Points[targetObjective-1].size()) {
    if (std::find(ZeroObjectives.begin(), ZeroObjectives.end(), targetObjective-1) != ZeroObjectives.end() ) {
      for (int i = 0; i < 6; i++) {
      Location[targetObjective-1][i] = 0;
      }
    }
    return 0;
  }
 
  //Check if current Position
  bool isZero = true;
  for (int i = 0; i < 6; i++) {
    if (Points[targetObjective-1][SequenceCounter][i] != 0) {
      //std::cout << "Points[" << targetObjective-1 << "][" << SequenceCounter << "][" << i << "] != 0\n";
      isZero = false;
    }
  }
  if (isZero) {
    for (int i = 0; i < 6; i++) {
        targetCoordinates[i] = Location[targetObjective-1][i];
    }
  }
  else {
    for (int i = 0; i < 6; i++) {
      targetCoordinates[i] = Points[targetObjective-1][SequenceCounter][i];     
    }
    coordTransform(targetCoordinates, TransMat[targetObjective-1]);
  }
  //printf("Coordinates: (%f,%f,%f,%f,%f,%f)\n", coordinates[0],coordinates[1],coordinates[2],coordinates[3],coordinates[4],coordinates[5]);
}

void PositionHandling::countSequence() {
  ++SequenceCounter;
}
void PositionHandling::resetSequence() {
  SequenceCounter = 0;
}


/*Prepares new Sequence. Takes currentPosition of Arm as Objective, if */
void PositionHandling::newTeachObjective(KinovaPts::Objective targetObjective, float* currentCoordinates) {
  if (std::find(ZeroObjectives.begin(), ZeroObjectives.end(), targetObjective-1) != ZeroObjectives.end() ) {
    std::cout << "newTeachObject is ZeroObjective!\n";
    for (int i = 0; i < 6; i++) {
    Location[targetObjective-1][i] = currentCoordinates[i];
    }
    calcTransMat();
  }
  resetSequence();
}


/*Saves coordinates to current Sequence Point or as Location in object.*/
void PositionHandling::savePoint(float coordinates[6], KinovaPts::Objective targetObjective) {
/*  bool isZero = true;
  for (int i = 0; i < 6; i++) {
    if (Points[targetObjective-1][SequenceCounter][i] != 0) {
      isZero = false;
    }
  }
  if (isZero) {
    for (int i = 0; i < 6; i++) {
      Location[targetObjective-1][i] = coordinates[i];
    }
    calcTransMat();
  }
  else {*/
    coordTransform(coordinates, InvTransMat[targetObjective-1]);
    for (int i = 0; i < 6; i++) {
      Points[targetObjective-1][SequenceCounter][i] = coordinates[i];
    }
  //}
}


/*Saves coordinates as origin of objectiv.*/
void PositionHandling::saveOrigin(float coordinates[6], KinovaPts::Objective targetObjective) {
  for (int i = 0; i < 6; i++) {
    Location[targetObjective-1][i] = coordinates[i];
  }
  calcTransMat();
}


/*Reads Location and Points Vectors from SaveFile*/
void PositionHandling::readFromFile() {
  std::vector<int>().swap(ZeroObjectives);
  std::string line;
  std::ifstream infile (FILEPATH);
  
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
        std::cout << "ReadFromFile: No Location. ERROR: File-Missmatch.";
        break;
      }
      bool isZero = true;
      for (int j = 0; j < 6; j++) {
        if (vec_int[j] != 0) { isZero = false; }
        Location[i][j] = (float)vec_int[j]/1000;
      }
      if (isZero) {
        ZeroObjectives.push_back(i);
        std::cout << "Objective " << i << " is Zero!\n";
      }
    }
  }

  //Empty Line
  std::getline(infile,line);
  
  //reset Point-Vector
  for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
    f2d_vec_t(0,std::vector<float>(6)).swap(Points[i]); 
  }
  
  //Reads Points from Files
  int location = 0;
  int sequence = 0;
  //bool PrePoint = true;
  f2d_vec_t::iterator it = Points[location].begin();
  
  while (std::getline(infile,line) && location < KinovaPts::NumberOfObjectives) { 
    //std::cout << "location: " << location << ", Sequence: " << sequence;
    std::istringstream iss(line);
    int n;
    std::vector<int> vec_int;
    vec_int.reserve(6);
    while (iss >> n) {
      vec_int.push_back(n);
    }
    if (vec_int.size() == 0) {
      //start new Objective.
      location++;
      sequence = 0;
      it = Points[location].begin();
      //PrePoint = true;
      //std::cout << ": Empty line. Next Location\n";
    }
    else {
      //save Point
      std::vector<float> vec_float(6);
      //bool isZero = true;
      for (int i = 0; i < 6; i++) {
        /*if (vec_int[i] != 0) {
          isZero = false;
        }*/
        vec_float[i] = (float)vec_int[i]/1000;
      }
      /*if (isZero) {
        PrePoint = false;
        //std::cout << ": Zero-Point\n";
      }
      else if (PrePoint) {
        Points[location].insert(it+sequence, vec_float);
        //std::cout << ": Pre-Zero\n";
      }
      else {*/
        Points[location].push_back(vec_float);
        //std::cout << ": Post-Zero\n";
      //}
      //std::cout << "Size at Location " << location << ": " << Points[location].size() << "\n";
      sequence++;
    }
  }
  std::cout << "File Loaded\n";
}


/*Writes Location and Points Vectors to SaveFile*/
void PositionHandling::writeToFile() {
  std::ofstream saveFile (FILEPATH);
  if ( saveFile.is_open() ) {
    //Write Locations
    for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
      if (std::find(ZeroObjectives.begin(), ZeroObjectives.end(), i) != ZeroObjectives.end() ) {
        //Objecitve is a ZeroObjective. 
        for (int j = 0; j < 6; j++) {
          saveFile << 0 << " ";
        }
      }
      else {
        //Normal Objective
        for (int j = 0; j < 6; j++) {
          saveFile << (int)(Location[i][j]*1000) << " ";
        }
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
  saveFile.close();
  std::cout << "File Saved\n";
}

int PositionHandling::getSequence() {
  return SequenceCounter;
}


/*Transforms coordinates Objective coordinate system to Basis coordinate system*/
void PositionHandling::coordTransform(float* coordinates, const f2d_vec_t &transMat) {
  //define point Vector ([x;y;z;1]) and angle Vector ([alpha, beta, gamma])
  f2d_vec_t cord(4, std::vector<float>(1,0));
  float ang[3];
  for (int i = 0; i<3; i++) {
    cord[i][0] = coordinates[i];
    ang[i] = coordinates[i+3];
  }
  cord[3][0] = 1;
  //Multiply Point Vector with TransformationMatrix
  cord = matMultiply(transMat, cord);

  //Get rotational Matrix from Objective in Base Coordinate System. (extracted from inverse transformation Matrix
  f2d_vec_t R1(3, std::vector<float>(3,0));
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j <3; j++) {
      R1[i][j] = transMat[i][j];
    }
  }
  //Get rotational Matrix from Point angles in Objective coordinate system.
  f2d_vec_t R2(3, std::vector<float>(3,0));
  R2 = rotMatrix(ang);
  //Calculate rotational Matrix from Point in Objective Coordinate System
  f2d_vec_t R3(3, std::vector<float>(3,0));
  R3 =  matMultiply(R1,R2);
  std::vector<float> angles(3);
  angles = getEulerAngles(R3);
  //write coordinates
  for (int i = 0; i<3; i++) {
    coordinates[i] = cord[i][0];
    coordinates[i+3] = angles[i];
  }
}


/*takes Array of 3 Angles alpha, beta, gamma and returns RotationMatrix of Euler XYZ*/
PositionHandling::f2d_vec_t PositionHandling::rotMatrix(float angle[3]) {
  f2d_vec_t mat(3, std::vector<float>(3,0) );
  double c[3], s[3];
  for (int i = 0; i < 3; i++) {
    c[i] = cos( angle[i] );
    s[i] = sin( angle[i] );
  }
  /*Hardcoded RotationMatrix of Euler XYZ*/
  mat[0][0] =  c[1]*c[2];
  mat[0][1] = -c[1]*s[2];
  mat[0][2] =  s[1];
  mat[1][0] =  c[0]*s[2]+s[0]*c[2]*s[1];
  mat[1][1] =  c[0]*c[2]-s[0]*s[2]*s[1];
  mat[1][2] = -s[0]*c[1];
  mat[2][0] =  s[0]*s[2]-c[0]*c[2]*s[1];
  mat[2][1] =  s[0]*c[2]+c[0]*s[2]*s[1];
  mat[2][2] =  c[0]*c[1];

  return mat;
}


/*Multiplies two matrices and returns resulting matrix.*/
PositionHandling::f2d_vec_t PositionHandling::matMultiply(const f2d_vec_t &mat1, const f2d_vec_t &mat2) {
  int d1 = mat1.size();
  int d2 = mat1[0].size();
  int d3 = mat2[0].size();

  f2d_vec_t ans(d1, std::vector<float>(d3,0));
  for (int i = 0; i < d1; i++) {
    for (int j = 0; j < d3; j++) {
      for (int k = 0; k < d2; k++) {
        ans[i][j] += mat1[i][k]*mat2[k][j]; 
      }
    }
  }
  return ans;
}


/*returns Euler Angles of Euler XYZ rotational matrix.*/
std::vector<float> PositionHandling::getEulerAngles(const f2d_vec_t rotMat) {
  std::vector<float> res(3);
  if (rotMat[1][3] < 1) {
    if(rotMat[1][3] > -1) {
      res[1] = asin( rotMat[0][2] );
      res[0] = atan2( -rotMat[1][2], rotMat[2][2] );
      res[2] = atan2( -rotMat[0][1], rotMat[0][0] );
    }
    else {
      res[1] = -1.5708;
      res[0] = -atan2( rotMat[1][0], rotMat[1][1] );
      res[2] = 0;
    }
  }
  else {
    res[1] = 1.5708;
    res[0] = atan2( rotMat[1][0], rotMat[1][1] );
    res[2] = 0;
  }
  return res;
}


/*Calculate Transformation-Matrices for every Objectives coordinate system*/
void PositionHandling::calcTransMat() {
  double p[3], c[3], s[3];
  //Calculate Sinus and Cosinus of all angles
  for (int i = 0; i < KinovaPts::NumberOfObjectives; i++) {
    for (int j =0; j<3; j++) {
    p[j] = Location[i][j];
    c[j] = cos( Location[i][j+3] );
    s[j] = sin( Location[i][j+3] );
    }

    //Hardcoded Euler XYZ Transformation Matrix
    TransMat[i][0][0] =  c[1]*c[2];
    TransMat[i][0][1] = -c[1]*s[2];
    TransMat[i][0][2] =  s[1];
    TransMat[i][0][3] =  p[0];
    TransMat[i][1][0] =  c[0]*s[2]+s[0]*c[2]*s[1];
    TransMat[i][1][1] =  c[0]*c[2]-s[0]*s[2]*s[1];
    TransMat[i][1][2] = -s[0]*c[1];
    TransMat[i][1][3] =  p[1];
    TransMat[i][2][0] =  s[0]*s[2]-c[0]*c[2]*s[1];
    TransMat[i][2][1] =  s[0]*c[2]+c[0]*s[2]*s[1];
    TransMat[i][2][2] =  c[0]*c[1];
    TransMat[i][2][3] =  p[2];
    TransMat[i][3][0] =  0;
    TransMat[i][3][1] =  0;
    TransMat[i][3][2] =  0;
    TransMat[i][3][3] =  1;

    //Hardcoded inverse of Euler XYZ Transformation Matrix
    //from [0][0] till [2][2] transformation matrix is rotational matrix 'R', as 'R' is orthagonal, R^-1=R^T
    for (int j = 0; j <3; j++) {
      for (int k = 0; k <3; k++) {
        InvTransMat[i][j][k] =  TransMat[i][k][j];
      }
    }
    InvTransMat[i][0][3] = -p[0]*c[1]*c[2] -
                            p[1]*(c[0]*s[2]+s[0]*c[2]*s[1]) +
                            p[2]*(c[0]*c[2]*s[1]-s[0]*s[2]);
    InvTransMat[i][1][3] =  p[0]*c[1]*s[2] +
                            p[1]*(s[0]*s[2]*s[1]-c[0]*c[2]) -
                            p[2]*(s[0]*c[2]+c[0]*s[2]*s[1]);
    InvTransMat[i][2][3] = -p[0]*s[1]+p[1]*s[0]*c[1]-p[2]*c[0]*c[1];
    InvTransMat[i][3][0] =  0;
    InvTransMat[i][3][1] =  0;
    InvTransMat[i][3][2] =  0;
    InvTransMat[i][3][3] =  1;
  }
  std::cout << "calculated Transformation Matrices\n";
}
