/* The main program. As of now it waits for a client to connect on the specified
 * port. Once a client connects, all the incoming messages are displayed.
 * Usage (for now):
 * ./executable <listenport>
 */

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include "Server4.h"
#include "ClientThread.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>

using namespace std;
/*
int readFirst(Socket* socket)
{
  string message;
	unsigned char inBuffer[200];
	int msgLength, msgCode, inBufLength = 0;
	stringstream out;

	inBufLength = socket->readBytes(inBuffer, inBufLength, 2);
	msgLength = ((inBuffer[0]) << 8) + (inBuffer[1]);

	if(msgLength == 0)
	  std::cout << "Error: invalid registration." << std::endl;

	inBufLength = clientSocket->readBytes(inBuffer, inBufLength, 2);
	msgCode = ((inBuffer[0]) << 8) + (inBuffer[1]);

  inBufLength = clientSocket->readBytes(inBuffer, inBufLength, msgLength - 4);
  out << msgCode;
  message = string(out.str() << (char*)inBuffer, msgLength - 4);

  return message;
}*/

int main(int argc, char* argv[])
{
  vector<Thread*> threads;
  Thread* thread;
  Socket *clientSocket, *listenSocket = new Socket;
  int port;

  port = atoi(argv[1]);

  listenSocket->bindTo(port);
  listenSocket->listenForConn();

  while(true)
  {
    clientSocket = listenSocket->acceptConn(); 
    thread = new Thread();
    threads.push_back(thread);
    thread->start(clientSocket, &threads);

 
  }

  //receive(clientSocket);

  shutdown(clientSocket->getSockfd(), 1);
	close(clientSocket->getSockfd());
	std::cout << "Teh end." << std::endl;
}
