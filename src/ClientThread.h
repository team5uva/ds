#ifndef CLIENTTHREAD_H_
#define CLIENTTHREAD_H_

#include "Thread.h"

using namespace std;

class ClientThread: public Thread
{
private:
  Socket* clientSocket;
  vector<ClientThread*>* cThreads;
  void run();
 

public:
  /* Start the thread. */
  void start(Socket* clientSocket, vector<ClientThread*>* cThreads);
};

#endif /* CLIENTTHREAD_H_ */
