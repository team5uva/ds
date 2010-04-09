
#ifndef THREAD_H_
#define THREAD_H_

#include "Server4.h"

using namespace std;

class Thread {
private:
  enum STATUS 
  {
	  SERVER,
	  CLIENT
	} type;

  pthread_mutex_t m_mutex;
  pthread_t m_thread;
  Socket* socket;
  vector<Thread*>* threads;

  static void *start_thread(void *obj);
  void determineType();
  void runServer();
  void runClient();

public:
  int getType();
  volatile bool m_stoprequested;
  volatile bool m_running;
  
	Thread();

	~Thread();

  /* Start the thread. */
  void start(Socket* socket, vector<Thread*>* threads);

  /* Stop the thread. */
  void stop();

};

#endif /* THREAD_H_ */
