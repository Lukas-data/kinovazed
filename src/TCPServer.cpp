#include "TCPServer.h"

#include <stdio.h>
#include <stdexcept>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using namespace std;



TCPServer::TCPServer() {
  portno = 51717;
  printf("using port #%d\n", portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error("ERROR opening socket");
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  
  messageLength = COMMAND_LENGTH + (DATA_PACKAGES*DATA_LENGTH);
  getDataCycleCounter = 20;
}

TCPServer::~TCPServer() {
  
}

void TCPServer::error(const char *msg)
{
  perror(msg);
  throw std::runtime_error(msg);
}

void TCPServer::sendTCP(int command, int data1, int data2, int data3) {  

  int n; 
  char buffer[COMMAND_LENGTH + (messageLength)];
  int m=sprintf( buffer, "%4d%6d%6d%6d", command, data1, data2, data3);
  if (m != messageLength)
    error("ERROR preparing message");
  if ( ( n = write( newsockfd, buffer, strlen(buffer) ) ) < 0 )
    error("Error writing to socket");
  //printf("Message Size: %d\n", n);
}

int TCPServer::readTCP() {
  memset(buffer, '\0', BUFFER_SIZE);
  
  char commandString[COMMAND_LENGTH];
  char dataString[DATA_PACKAGES][DATA_LENGTH];

  int n= read(newsockfd, buffer, messageLength);

  if(n==0 && getDataCycleCounter > 0) {
    getDataCycleCounter -= getDataCycleCounter;
    commandRecieved = 0;
    return 1;  
  }
  else if (n==0 && getDataCycleCounter <= 0) {
    error("ERROR: Connection Lost");
    getDataCycleCounter = 20;
    return -1;
  }
  else if (n != messageLength) {
    error("Error reading from socket");
    getDataCycleCounter = 20;
    return -1;
  }

  memcpy(commandString, buffer, COMMAND_LENGTH);
  commandRecieved = atoi(buffer);
  for(int i = 0; i < DATA_PACKAGES; i++) {
    memcpy(dataString[i], buffer + COMMAND_LENGTH + DATA_LENGTH*i, DATA_LENGTH);
    dataRecieved[i] = atoi(dataString[i]);
  }
  getDataCycleCounter = 20;
  return 1;
}


void TCPServer::waitForClient() {
  //--- wait on a connection ---
  printf ( "waiting for new client...\n");
  if ( ( newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen)) <0)
    error("ERROR on accept");
  printf("opened new communication with client \n");
}


int TCPServer::getCommand() {
  if (readTCP() == 1)
    return commandRecieved;
  else
    return 99;
}


int TCPServer::getData(int dataPackage) {
  if (dataPackage<0 || dataPackage > 2) {
    printf("no data[%d] in communication protocol",dataPackage);
    return 0;
  }
  return dataRecieved[dataPackage];
}

int TCPServer::closeOnCommand(int command) {
  close(newsockfd);
  if ( command == -2)
    return -1;
  return 0;
  
}


