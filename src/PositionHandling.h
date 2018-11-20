#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_

#define NUMBER_OF_POINTS 2
#define NUMBER_OF_SUBPOINTS 2

namespace KinovaPts {

  enum Points { HOME,
                BELL
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
    bool getCoordinates(float* coordinates, KinovaPts::Points targetPoint);
    void countSequence();

  private:
    static KinovaPts::posCoordinates Positions[NUMBER_OF_POINTS][NUMBER_OF_SUBPOINTS];
    int SequenceCounter;
    
 

};


#endif
