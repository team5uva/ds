/* The main program. As of now it waits for a client to connect on the specified
 * port. Once a client connects, all the incoming messages are displayed.
 * Usage (for now):
 * ./executable <listenport>
 */

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <pthread.h>
#include "Server4.h"
#include "Server.h"
#include "configFile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Admin.h"
#include "Thread.h"
#include "Message.h"
#include "Thread.h"
#include "MessageType.h"

using namespace std;

void* controlThread(void *_obj) {
	string message = "87.210.237.85:2001:45j45t5h5t948dj5fh049ffe454Yo";

	Message m;
	m.type = ADDRESS_TO_CONTROL;
	m.addParameter(message);
	m.buildRawData();

	Socket controlServer_socket;
	controlServer_socket.connectTo("146.50.1.95", 2001);
	Message::MessageToSocket(&controlServer_socket, &m);

	Message* response;
	while (true) {
		response = Message::messageFromSocket(&controlServer_socket);
		response->parseData();

		cout << "Message type: " << response->type << "\n\n";

		// Get adres of the parent server if exists
		if (response->type == ADDRESS_FROM_CONTROL) {

			if(response->words[0] != "none"){
				cout << "Got parent server address: " << response->words[0] << "\n\n";

				//Create parent server
				Server* parent;
				parent->socketaddress = response->words[0];

				//server4->servers.push_back(parent);

			}else{
				cout << "No parent server exists.""\n\n";
			}

	    // PING - PONG control server
		} else if (response->type == PING) {
			Message msg;
			msg.type = PONG;
			msg.buildRawData();

			Message::MessageToSocket(&controlServer_socket, &msg);
		}
	}
	return NULL;
}

int main(int argc, char* argv[])
{


  vector<Thread*> threads;
  Thread* thread;
  Socket *clientSocket, *listenSocket = new Socket;
  configFile config;
  int port;

  pthread_t control_thread;
  int error = pthread_create(&control_thread, 0, controlThread, 0);

  /* Read configuration file */
  if(config.parseFile() == configFile::SUCCESS) {
      /* No problems detected in configuration file */
      /* Maybe some detection here to check if all variables have indeed been set*/
      cout << "No problems detected in configuration file." << endl;
      for(int i = 0; i < config.adminAccess.size(); i++) {
          cout << config.adminAccess.at(i)->name << endl;
          cout << config.adminAccess.at(i)->password << endl;
      }
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

  Server4 server4;

  while(true)
  {
    clientSocket = listenSocket->acceptConn(); 
    thread = new Thread();
    threads.push_back(thread);
    thread->start(clientSocket, &threads, &server4);

 
  }

  //receive(clientSocket);

  shutdown(clientSocket->getSockfd(), 1);

	close(clientSocket->getSockfd());
	std::cout << "Teh end." << std::endl;
}
