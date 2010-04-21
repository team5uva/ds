#include "Socket.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

using namespace std;

Socket::Socket() {
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) // did not get valid socket
	{
		std::cout << "failed to acquire socket" << std::endl;
		exit(0);
	}
	sockState = NEW;
}

Socket::Socket(int sockfd) {
	this->sockfd = sockfd;
	sockState = NEW;
}

Socket::~Socket() {
	shutdown(sockfd, 1);
	close(sockfd);
}

/* Binds socket to port */
int Socket::bindTo(int port) {
	if (sockState != NEW)
		return -1;
	struct sockaddr_in address = { };

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		std::cout << "Error: Failed to bind port to socket" << std::endl;
		return -1;
	}

	sockState = BOUND;
	return 0;
}

/* Connects to port on different machine */
int Socket::connectTo(string host, int port) {
	if (sockState != NEW)
		return -1;

	char* cstr = new char[host.size() + 1];
	struct sockaddr_in address = { };
	struct hostent *server;

	strcpy(cstr, host.c_str());

	server = gethostbyname(cstr);
	if (server == NULL) {
		std::cout << "Error: No such host." << std::endl;
		return -1;
	}
	address.sin_family = AF_INET;
	bcopy((char*) server->h_addr, (char*) &address.sin_addr.s_addr, server->h_length);
	address.sin_port = htons(port);
	if (connect(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
		std::cout << "Error: Failed to connect to socket." << std::endl;
		return -1;
	}

	sockState = CONNECTED;
	return 0;
}

/* Blocks thread till external connection attempt is made */
Socket* Socket::acceptConn() {
	if (sockState != LISTENING)
		return NULL;

	struct sockaddr_in client_address;
	socklen_t client_length = sizeof(client_address);

	return new Socket(accept(sockfd, (struct sockaddr*) &client_address, &client_length));
}

int Socket::listenForConn() {
	if (sockState == BOUND) {
		listen(sockfd, 5);
		sockState = LISTENING;
		return 0;
	} else
		return -1;
}

/* Reads bytes and removes them from queue if there are bytes to read, else
 * blocks thread. prevReadLen is the amount of chars currently in inBuffer.
 */
int Socket::readBytes(unsigned char* inBuffer, int prevReadLen, int bytes) {
	bzero(inBuffer, prevReadLen);
	int result = read(sockfd, inBuffer, bytes);

	if (!result) {
		throw SocketException("Could not read from socket.");
	}
	return result;
}

/* Reads bytes and leaves them in queue, does not block thread.
 * prevReadLen is the amount of chars currently in inBuffer.
 */
int Socket::peekBytes(unsigned char* inBuffer, int prevReadLen, int bytes) {
	bzero(inBuffer, prevReadLen);
	return recv(sockfd, inBuffer, bytes, MSG_PEEK);
}

int Socket::writeBytes(unsigned char* buffer, int bytes) {
	return write(sockfd, buffer, bytes);
}

/* Gets socket file descriptor */
int Socket::getSockfd() {
	return sockfd;
}

/* Gets socket state. Return values:
 * 1 = New
 * 2 = Bound
 * 3 = Connected
 * 4 = Listening
 */
int Socket::getSockstate() {
	return sockState;
}

/* Gets bound/connected port */
int Socket::getPort() {
	return port;
}

int Socket::send(MESSAGE message) {
	int status = writeBytes((unsigned char*) &message, ntohs(message.len));
	if (status == -1) {
		return false;
	} else {
		cout << "message " << ntohs(message.type) << " < " << ntohs(message.len) << " is sent. \n";
		return true;
	}
}

int Socket::receive(MESSAGE &message) {

	int status = readBytes((unsigned char*) &message, 0, 200);

	if (status == -1) {
		std::cout << "status == -1   error  " << "  in Socket::recv\n";
		return 0;
	} else if (status == 0) {
		return 0;
	} else {
		return status;
	}
}
