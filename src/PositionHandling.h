#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#include <vector>

#define NUMBER_OF_SUBPOINTS 10   //Corresponds with the maximum length of a Sequence.

namespace KinovaPts {
  
  static const int NumberOfObjectives = 2; //excl. NoObjective
  enum Objective { NoObjective,
                    Home,
                    Bell
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
      Points(KinovaPts::NumberOfObjectives, cord_vec_t(1,std::vector<float>(6)))
      {}
    ~PositionHandling();
    static int printPos();
    bool getCoordinates(float* coordinates, KinovaPts::Objective targetObjective);
    void countSequence();
    void resetSequence();
    void savePoint(float coordinates[6], KinovaPts::Objective targetObjective);
    void readFromFile();
    void writeToFile();
    int getSequence();

  private:
    typedef std::vector< std::vector<float> > cord_vec_t;
    typedef std::vector<cord_vec_t> seq_vec_t;
    static KinovaPts::posCoordinates Locations[KinovaPts::NumberOfObjectives][NUMBER_OF_SUBPOINTS];
    cord_vec_t Location;
    seq_vec_t Points;
    int SequenceCounter;

};


#endif
