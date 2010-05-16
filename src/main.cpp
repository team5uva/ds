/*
 * The main program.
 */
#include <iostream>
#include <pthread.h>
#include "Socket.h"
#include "Server4.h"
#include "Thread.h"

int main(int argc, char* argv[]) {
  Thread* thread;
  Socket *clientSocket = new Socket;

  Server4 server4;
  pthread_t controlServer_thread;

  /* Start the control server thread. */
  pthread_create(&controlServer_thread, 0, Server4::controlThread, &server4);

  /* Accept client/server connections and create seperate thread for each
   * connection
   */
  while (true) {
    clientSocket = server4.listenSocket->acceptConn();
    thread = new Thread();
    thread->start(clientSocket, &server4);
  }
}
