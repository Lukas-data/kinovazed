#ifndef _POSHANDLING_H_
#define _POSHANDLING_H_



using namespace std;

#define NUMBER_OF_POINTS 1

enum Points { HOME = 0,
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


    static posCoordinates Positions[NUMBER_OF_POINTS];
    
    void error(const char *msg);

	public:
	  
    static int printPos();
    static void getCoordinates(float* coordinates, int pos);
    
 

};
#endif
