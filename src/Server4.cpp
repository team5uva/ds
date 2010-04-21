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
#include "configFile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Admin.h"
#include "Message.h"
#include "Thread.h"

using namespace std;

void* controlThread(void *_obj) {

	MESSAGE msg;

	string message = "87.210.237.85:2001:45j45t5h5t948dj5fh049ffe454Yo";

	for (unsigned int i = 0; i < message.size(); i++) {
		msg.msg[i] = message.at(i);
	}

	msg.len = htons(message.length() + 4);
	msg.type = htons(601);

	Socket controlServer_socket;
	controlServer_socket.connectTo("146.50.1.95", 2001);
	controlServer_socket.send(msg);

	MESSAGE response;
	while (true) {

		if (controlServer_socket.receive(response) > 0) {

			cout << "Mss: " << ntohs(response.len) << " " << ntohs(response.type) << " " << response.msg << "\n\n";

			if (ntohs(response.type) == 140) {

				msg.type = htons(150);
				msg.len = response.len;

				for (unsigned int i = 0; i < ntohs(response.type) - 4; i++) {
					msg.msg[i] = response.msg[i];
				}

				controlServer_socket.send(msg);
			}
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
