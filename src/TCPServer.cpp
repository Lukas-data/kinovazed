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
#include "Log.h"


#define RIO_DUMMY false


//TODO: different Errorhandling!
void TCPServer::error(const char *funcName, const char *msg)
{
  ALL_LOG(logERROR) << "TCPServer::" << funcName << "(): " << msg;
}

/*Opens TCP Server and establishes connection to a client(RoboRio)*/
bool TCPServer::connect() {
  if (RIO_DUMMY == false) {
    if(clilen == 0) {
      portno = 51717;
      sockfd = socket(AF_INET, SOCK_STREAM, 0);
      if (sockfd < 0) {
        error("connect","Error opening socket");
        return false;
      }

      memset(&serv_addr, 0, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;
      serv_addr.sin_port = htons(portno);
      serv_addr.sin_addr.s_addr = INADDR_ANY;

      if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("connect","Error on binding");
        return false;
      }
      listen(sockfd,5);
      clilen = sizeof(cli_addr);
    }

    // Wait for a connection with a client
    if ( ( newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t*) &clilen)) <0) {
      error("connect","Error on accept");
      return false;
    }
  }
  return true;
}

/*sends Message to Client (RoboRio). Takes Command and data packages as input*/
bool TCPServer::sendTCP(int command, int eventVar, int data1, int data2, int data3) {  
  if (RIO_DUMMY == false) {
    ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(): start";
    int n; 
    char buffer[COMMAND_LENGTH + (MessageLength)];
    int m=sprintf( buffer, "%6d%6d%6d%6d%6d", command, eventVar, data1, data2, data3);
    if (m != MessageLength) {
      error("sendTCP","Error preparing message");
      return false;
    }
    ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(): Message prepared.";
    if ( ( n = write( newsockfd, buffer, strlen(buffer) ) ) < 0 ) {
      error("sendTCP","Error writing to socket");
      return false;
    }
  }
  ALL_LOG(logDEBUG4) << "TCPServer::sendTCP(" << command << ", "
                                              << eventVar << ", "
                                              << data1 << ", "
                                              << data2 << ", "
                                              << data3 << ")" ;
  return true;
}

/*reads Message from Client (RoboRio). Saves Command to commandRecieved
and Data to dataRecieved[]*/
bool TCPServer::readTCP() {
  if (RIO_DUMMY == false) {
    memset(buffer, '\0', BUFFER_SIZE);
    
    char commandString[COMMAND_LENGTH];
    char dataString[DATA_PACKAGES][DATA_LENGTH];

    int n= read(newsockfd, buffer, MessageLength);

    if(n==0 && NoDataCycleCount > 0) {
      --NoDataCycleCount;
      commandRecieved = 0;
      return true;
    }
    else if (n==0 && NoDataCycleCount <= 0) {
      error("readTCP","Connection Lost");
      NoDataCycleCount = NOCONNECTION_COUNT;
      return false;
    }
    else if (n != MessageLength) {
      error("readTCP","Error reading from socket");
      NoDataCycleCount = NOCONNECTION_COUNT;
      return false;
    }

    memcpy(commandString, buffer, COMMAND_LENGTH);
    commandRecieved = atoi(buffer);
    for(int i = 0; i < DATA_PACKAGES; i++) {
      memcpy(dataString[i], buffer + COMMAND_LENGTH + DATA_LENGTH*i, DATA_LENGTH);
      dataRecieved[i] = atoi(dataString[i]);
    }
    NoDataCycleCount = NOCONNECTION_COUNT;
  }
  return true;
}


int TCPServer::getCommand() {
  if ( readTCP() )
    return commandRecieved;
  else
    return 99;
}


int TCPServer::getData(int dataPackage) {
  if (dataPackage<0 || dataPackage > DATA_PACKAGES-1) {
    ALL_LOG(logDEBUG4) << "no data[" << dataPackage << "] in communication protocol.";
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


