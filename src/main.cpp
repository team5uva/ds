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
//	std::cout << "test";
//	std::cout << "test";


	server4.listenSocket = new Socket;
	server4.port = server4.listenSocket->bindTo(server4.port);
	server4.listenSocket->listenForConn();
	int error = pthread_create(&controlServer_thread, 0, Server4::controlThread, &server4);

	while (true) {
	        std::cout << "about to start block for listening" << std::endl;
		clientSocket = server4.listenSocket->acceptConn();
		std::cout << "about to create thread" << std::endl;
		thread = new Thread();
		thread->start(clientSocket, &server4);
	}
	delete clientSocket;

	std::cout << "Teh end." << std::endl;
}
