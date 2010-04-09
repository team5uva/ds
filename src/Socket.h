
#ifndef SOCKET_H_
#define SOCKET_H_

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

using namespace std;

class Socket
{

private:
  int sockState;
  int sockfd;
  int port;

  enum STATUS
  {
    NEW = 0,
    BOUND = 1,
    CONNECTED = 2,
    LISTENING = 3
	};

public:
	Socket()
  {
    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) // did not get valid socket
    {
      std::cout << "failed to acquire socket" << std::endl;
      exit(0);
    }
    sockState = NEW;
  }

  Socket(int sockfd)
  {
    this->sockfd = sockfd;
    sockState = NEW;
  }

	~Socket()
  {
    shutdown(sockfd, 1);
	  close(sockfd);
  }

  /* Binds socket to port */
  int bindTo(int port)
  {
    if(sockState != NEW)
      return -1;
    struct sockaddr_in address = {};

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if(bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
      std::cout << "Error: Failed to bind port to socket" << std::endl;
      return -1;
    }

    sockState = BOUND;
    return 0;
  }

  /* Connects to port on different machine */
  int connectTo(string host, int port)
  {
    if(sockState != NEW)
      return -1;

    char* cstr = new char [host.size() + 1];
    struct sockaddr_in address = {};
    struct hostent *server;

    strcpy(cstr, host.c_str());

    server = gethostbyname(cstr);
    if(server == NULL)
    {
      std::cout << "Error: No such host." << std::endl;
      return -1;
    }
    address.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&address.sin_addr.s_addr, server->h_length);
    address.sin_port = htons(port);
    if(connect(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0)
    {
      std::cout << "Error: Failed to connect to socket." << std::endl;
      return -1;
    }

    sockState = CONNECTED;
    return 0;
  }

  /* Blocks thread till external connection attempt is made */
  Socket* acceptConn()
  {
    if(sockState != LISTENING)
      return NULL;
    
    struct sockaddr_in client_address;
	  socklen_t client_length = sizeof(client_address);

    return new Socket(accept(sockfd, (struct sockaddr*) &client_address, &client_length));
  }
  
  int listenForConn()
  {
    if(sockState == BOUND)
    {
      listen(sockfd, 5);
      sockState = LISTENING;
      return 0;
    }
    else
      return -1;
  }

  int readBytes(unsigned char* inBuffer, int prevReadLen, int bytes)
  {
    bzero(inBuffer, prevReadLen);
    return read(sockfd, inBuffer, bytes);
  }



  /* Gets socket file descriptor */
  int getSockfd()
  {
    return sockfd;
  }
  
  /* Gets socket state. Return values:
   * 1 = New
   * 2 = Bound
   * 3 = Connected
   * 4 = Listening
   */
  int getSockstate()
  {
    return sockState;
  }

  /* Gets bound/connected port */
  int getPort()
  {
    return port;
  }
};

#endif /* SERVER_H_ */
