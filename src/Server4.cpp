/* The main program. As of now it waits for a client to connect on the specified
 * port. Once a client connects, all the incoming messages are displayed.
 * Usage (for now):
 * ./executable <listenport>
 */

#include <iostream>
#include <string>
#include <cstring>
#include "Server4.h"
#include "ClientThread.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

void receive(Socket* clientSocket)
{
  string message;
  unsigned char inBuffer[200];
  int msgLength, msgCode, inBufLength = 0;

  while(true)
  {
    inBufLength = clientSocket->readBytes(inBuffer, inBufLength, 2);
    msgLength = ((inBuffer[0]) << 8) + (inBuffer[1]);

    if(msgLength == 0)
      break;

    inBufLength = clientSocket->readBytes(inBuffer, inBufLength, 2);
    msgCode = ((inBuffer[0]) << 8) + (inBuffer[1]);

    if(msgLength - 4 > 0)
    {
      inBufLength = clientSocket->readBytes(inBuffer, inBufLength, msgLength - 4);
      message = string((char*)inBuffer, msgLength - 4);
    }
    else
      message = string();

    //std::cout << "Message length: " << msgLength << std::endl;
    std::cout << "Message code: " << msgCode << std::endl;
    std::cout << "Message: " << message << std::endl;
  }
}

int main(int argc, char* argv[])
{
  vector<ClientThread*> cThreads;
  ClientThread* cThread;
  Socket *clientSocket, *listenSocket = new Socket;
  int port;

  port = atoi(argv[1]);

  listenSocket->bindTo(port);
  listenSocket->listenForConn();

  while(true)
  {
    clientSocket = listenSocket->acceptConn(); //TODO: loop this line forever and create seperate receive thread for each accepted connection
    cThread = new ClientThread();
    cThread->start(clientSocket);
  }

  //receive(clientSocket);

  shutdown(clientSocket->getSockfd(), 1);
	close(clientSocket->getSockfd());
	std::cout << "Teh end." << std::endl;
}
