
#ifndef THREAD_H_
#define THREAD_H_

#include "Server4.h"

using namespace std;

class Thread
{
private:
  enum STATUS 
  {
	  SERVER,
	  CLIENT
	} type;

  pthread_mutex_t m_mutex;
  pthread_t m_thread;
  Socket* socket;
  Server4* server4;
  time_t lastActivityTime;
  bool waiting_for_pong;
  Message* latestBroadcast;

  static void *start_thread(void *obj);
  void determineType();
  void runServer(Server* s);
  void runClient(Client* c);
  void processClientMessage(Client* c, Message* msg);
  void processClientBroadcast(Client* c, Message* msg);
  void processServerMessage(Server* s, Message* m);
  void processServerBroadcast(Server* s, Message* m);
  void ping();

public:
  int getType();
  volatile bool m_stoprequested;
  volatile bool m_running;
  
	Thread();

	~Thread();

  /* Start the thread. */
  void start(Socket* socket, Server4* server);

  /* Stop the thread. */
  void stop(bool isClient);

};

#endif /* THREAD_H_ */
