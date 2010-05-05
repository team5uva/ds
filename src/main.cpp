#include <iostream>
#include "Socket.h"
#include "Server4.h"
#include "Thread.h"

int main(int argc, char* argv[]) {
	Thread* thread;
	Socket *clientSocket = new Socket;

	Server4 server4;

	server4.listenSocket = new Socket;
	server4.port = server4.listenSocket->bindTo(server4.port);
	server4.listenSocket->listenForConn();

	while (true) {
		clientSocket = server4.listenSocket->acceptConn();
		thread = new Thread();
		thread->start(clientSocket, &server4);
	}
	delete clientSocket;

	std::cout << "Teh end." << std::endl;
}
