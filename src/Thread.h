
#ifndef THREAD_H_
#define THREAD_H_

#include "Server4.h"

using namespace std;

class Thread {
private:
  pthread_mutex_t m_mutex;
  pthread_t m_thread;

  static void *start_thread(void *obj)
  {
    reinterpret_cast<Thread*>(obj)->run();
  }

  virtual void run() { }

public:
  volatile bool m_stoprequested;
  volatile bool m_running;
  
	Thread() : m_stoprequested(false), m_running(false)
  {
    pthread_mutex_init(&m_mutex, 0);
  }

	~Thread()
  {
    pthread_mutex_destroy(&m_mutex);
  }

  /* Start the thread. */
  void start()
  {
    assert(m_running == false);
    m_running = true;
    pthread_create(&m_thread, 0, &Thread::start_thread, this);
  }

  /* Stop the thread. */
  void stop()
  {
    assert(m_running == true);
    m_running = false;
    m_stoprequested = true;
    pthread_join(m_thread, 0);
  }

};

#endif /* THREAD_H_ */
