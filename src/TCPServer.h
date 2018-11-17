#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <netinet/in.h>


#define BUFFER_SIZE 256
#define COMMAND_LENGTH 4
#define DATA_LENGTH 6
#define DATA_PACKAGES 3
#define NOCONNECTION_COUNT 20


struct Command {
  enum Name {
    Nothing = 0,
    Init = 1,
    SetMode = 2,
    Steer = 3,
    MoveToPos = 4,
    PosReached = 5,
    EStop = 91,
    EStop_Quit = 92,
    Error = 99
  };
};


class TCPServer {

	public:
	  TCPServer() :
      MessageLength(COMMAND_LENGTH + (DATA_PACKAGES*DATA_LENGTH)),
      NoDataCycleCount(NOCONNECTION_COUNT),
      clilen(0)
      {}      

    bool connect();

    int  getCommand();
    int  getData(int dataPackage); //dataPackage = number of the requested Package (0-DATA_PACKAGES-1).
    int  closeOnCommand(int command);
    bool  readTCP();
    bool sendTCP(int command, int data1, int data2, int data3);

  private:

    void error(const char *msg);

    int  sockfd, newsockfd, portno, clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    int  commandRecieved;
    int  commandToSend;
    int  dataRecieved[DATA_PACKAGES];
    int  dataToSend[DATA_PACKAGES];

    int  MessageLength;
    int  NoDataCycleCount;



};
#endif
