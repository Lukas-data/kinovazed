#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <netinet/in.h>


#define BUFFER_SIZE 256
#define COMMAND_LENGTH 6
#define DATA_LENGTH 6
#define DATA_PACKAGES 4
#define NOCONNECTION_COUNT 20


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
    bool readTCP();
    bool sendTCP(int command, int eventVar, int data1, int data2, int data3);

  private:

    void error(const char *funcName, const char *msg);

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
