#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#define NUMBER_OF_SUBPOINTS 10   //Corresponds with the maximum length of a Sequence.

namespace KinovaPts {
  
  static const int NumberOfPositions = 2; //excl. NoPosition
  enum Positions { NoPosition,
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
      SequenceCounter(0)
      {}
    ~PositionHandling();
    static int printPos();
    bool getCoordinates(float* coordinates, KinovaPts::Positions targetPosition);
    void countSequence();
    void resetSequence();
    void savePoint(float coordinates[6], KinovaPts::Positions object);
    void readFromFile();
    void writeToFile();
    int getSequence();

  private:
    static KinovaPts::posCoordinates Positions[KinovaPts::NumberOfPositions][NUMBER_OF_SUBPOINTS];
    int SequenceCounter;

};


#endif
