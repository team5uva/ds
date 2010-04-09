#include "ClientThread.h"
#include <iostream>

using namespace std;


void ClientThread::run()
{
  while (!m_stoprequested)
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
}

/* Start the thread. */
void ClientThread::start(Socket* clientSocket)
{
  this->clientSocket = clientSocket;
  Thread::start();
}

