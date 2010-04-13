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
#include "configFile.h"
#include "ClientThread.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

int main(int argc, char* argv[])
{
  vector<ClientThread*> cThreads;
  ClientThread* cThread;
  Socket *clientSocket, *listenSocket = new Socket;
  configFile config;
  int port;

  /* Read configuration file */
  if(config.parseFile() == configFile::SUCCESS) {
      /* No problems detected in configuration file */
      /* Maybe some detection here to check if all variables have indeed been set*/
      cout << "No problems detected in configuration file." << endl;
  } else {
      /* Error in configuration file, shut down program */
      /* Maybe make a breakdown of different sort of errors to handle it more
         gracefully */
      cout << "Problem detected in configuration file, shutting down..." << endl;
      return 0;
  }
  port = atoi(argv[1]);

  listenSocket->bindTo(port);
  listenSocket->listenForConn();

  while(true)
  {
    clientSocket = listenSocket->acceptConn(); 
    cThread = new ClientThread();
    cThread->start(clientSocket);
  }

  //receive(clientSocket);

  shutdown(clientSocket->getSockfd(), 1);
	close(clientSocket->getSockfd());
	std::cout << "Teh end." << std::endl;
}
