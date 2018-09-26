#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include <netinet/in.h>

#define BUFFER_SIZE 256
#define COMMAND_LENGTH 4
#define DATA_LENGTH 6
#define DATA_PACKAGES 3

using namespace std;

class TCPServer {

  private:
    int sockfd, newsockfd, portno, clilen;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
//    int data;
    int commandRecieved;
    int commandToSend;
    int dataRecieved[DATA_PACKAGES];
    int dataToSend[DATA_PACKAGES];
//    int commandLength;
//    int dataLength;
    int messageLength;

    void error(const char *msg);
    int getDataCycleCounter;

	public:
    //enum Commands {SET_MODE =1, SET_MODE = 2};
	  TCPServer();
    ~TCPServer();
    void waitForClient();
    int getCommand();
    int getData(int dataPackage); //dataPackage = number of the requested Package (0-DATA_PACKAGES-1).
    int closeOnCommand(int command);
    int readTCP();
    void sendTCP(int command, int data1, int data2, int data3);
  

};
#endif
