#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include <vector>

#define NUMBER_OF_SUBPOINTS 10   //Corresponds with the maximum length of a Sequence.

namespace KinovaPts {
  
  static const int NumberOfObjectives = 4; //excl. NoObjective
  enum Objective {  NoObjective,
                    Home,
                    Bell,
                    Handle,
                    OpenDoor
                 };

  typedef struct posCoordinates{ 
    float x; //Kinova Left/Right(-)  0.8..-0.8
    float y; //Kinova Front(-)/Back  -0.85..0.85
    float z; //Kinova Up  1.15..
    float pitch;
    float yaw;
    float roll;
  }posCoordinates;
}

class PositionHandling {

  public:
    PositionHandling() :
      SequenceCounter(0),
      Location(KinovaPts::NumberOfObjectives, std::vector<float>(6)),
      Points(KinovaPts::NumberOfObjectives, f2d_vec_t(0,std::vector<float>(6))),
      TransMat(KinovaPts::NumberOfObjectives, f2d_vec_t(4,std::vector<float>(4))),
      InvTransMat(KinovaPts::NumberOfObjectives, f2d_vec_t(4,std::vector<float>(4)))
      {}
    ~PositionHandling();
    
    void init();
    static int printPos();
    bool getCoordinates(float* coordinates, KinovaPts::Objective targetObjective, float* currentCoordinates);
    bool getOrigin(float* coordinates, KinovaPts::Objective targetObjective, float* currentCoordinates);
    void incrementSequence();
    void decrementSequence();
    void resetSequence();
    void newTeachObjective(KinovaPts::Objective targetObjective, float* currentCoordinates);
    bool savePoint(float coordinates[6], KinovaPts::Objective targetObjective);
    void saveOrigin(float coordinates[6], KinovaPts::Objective targetObjective);
    int  getSequence();
    void readFromFile();
    void writeToFile();

  private:
    typedef std::vector< std::vector<float> > f2d_vec_t;
    typedef std::vector<f2d_vec_t> f3d_vec_t;
    //static KinovaPts::posCoordinates Locations[KinovaPts::NumberOfObjectives][NUMBER_OF_SUBPOINTS];
    f2d_vec_t Location;
    f3d_vec_t Points;
    int SequenceCounter;
    f3d_vec_t TransMat;
    f3d_vec_t InvTransMat;
    std::vector<int> ZeroObjectives;

    void coordTransform(float* coordinates, const f2d_vec_t &transMat);
    void coordBackTransform(float* coordinates,KinovaPts::Objective targetObjective);
    f2d_vec_t rotMatrix(float angle[3]);
    f2d_vec_t matMultiply(const f2d_vec_t &mat1, const f2d_vec_t &mat2);
    std::vector<float> getEulerAngles(const f2d_vec_t rotMat);
    void calcTransMat();
    

};


#endif
