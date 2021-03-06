#include "Socket.h"
#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <errno.h>

using namespace std;

/* Socket constructor. */
Socket::Socket() {
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  sockState = NEW;
}

/* Socket constructor. */
Socket::Socket(int sockfd) {
  this->sockfd = sockfd;
  sockState = NEW;
}

/* Socket destructor. */
Socket::~Socket() {
  shutdown(sockfd, 1);
  close(sockfd);
}

/* Binds socket to port, returns bound port. Returns -1 on failure. */
int Socket::bindTo(int port) {
  int initialPort = port;
  bool bound = false;

  if (sockState != NEW)
    return -1;
  struct sockaddr_in address = {};

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  while (!bound) {
    address.sin_port = htons(port++);
    bound = bind(sockfd, (struct sockaddr *) & address, sizeof (address)) >= 0;

    if (initialPort < port - 10)
      return -1;
  }

  sockState = BOUND;
  return port - 1;
}

/* Connects to port on different machine */
int Socket::connectTo(string host, int port) {
  if (sockState != NEW)
    return -1;

  char* cstr = new char[host.size() + 1];
  struct sockaddr_in address = {};
  struct hostent *server;

  strcpy(cstr, host.c_str());

  server = gethostbyname(cstr);
  if (server == NULL) {
    return -1;
  }
  address.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) & address.sin_addr.s_addr, server->h_length);
  address.sin_port = htons(port);
  if (connect(sockfd, (struct sockaddr *) & address, sizeof (address)) < 0) {
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
  socklen_t client_length = sizeof (client_address);

  return new Socket(accept(sockfd, (struct sockaddr*) & client_address, &client_length));
}

/* Puts socket in listen mode. */
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

  return result;
}

/* Reads bytes and leaves them in queue.
 * prevReadLen is the amount of chars currently in inBuffer.
 * blocking variable decides whether call is blocking.
 */
int Socket::peekBytes(unsigned char* inBuffer, int prevReadLen, int bytes, bool blocking) {
  bzero(inBuffer, prevReadLen);
  return recv(sockfd, inBuffer, bytes, MSG_PEEK | (blocking ? 0 : MSG_DONTWAIT));
}

/* Writes bytes to socket. */
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

