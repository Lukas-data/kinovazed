#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_



//using namespace std;

#define NUMBER_OF_POINTS 2
#define NUMBER_OF_SUBPOINTS 2

enum Points { HOME = 0,
              BELL = 1
            };

typedef struct posCoordinates{ 
  float x;
  float y;
  float z;
  float pitch;
  float yaw;
  float roll;
}posCoordinates;


class PositionHandling {

  private:
    PositionHandling();
    ~PositionHandling();


    static posCoordinates Positions[NUMBER_OF_POINTS][NUMBER_OF_SUBPOINTS];
    //static posCoordinates homePositions[1];
    //static posCoordinates bellPositions[2];


    //void error(const char *msg);
    
    

	public:
	  
    static int printPos();
    static bool getCoordinates(float* coordinates, int pos, int sequence);
    
 

};
#endif
