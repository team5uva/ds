
#ifndef SOCKET_H_
#define SOCKET_H_
#include <string>

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
  Socket();
  Socket(int sockfd);

  ~Socket();
  
  /* Binds socket to port */
  int bindTo(int port);
   /* Connects to port on different machine */
  int connectTo(string host, int port);
  /* Blocks thread till external connection attempt is made */
  Socket* acceptConn();
 
  int listenForConn();
  int readBytes(unsigned char* inBuffer, int prevReadLen, int bytes);
  int peekBytes(unsigned char* inBuffer, int prevReadLen, int bytes);
  

  /* Gets socket file descriptor */
  int getSockfd();
  
  /* Gets socket state. Return values:
   * 1 = New
   * 2 = Bound
   * 3 = Connected
   * 4 = Listening
   */
  int getSockstate();

  /* Gets bound/connected port */
  int getPort();
};

#endif /* SERVER_H_ */
