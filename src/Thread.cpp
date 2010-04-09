#include "Thread.h"

using namespace std;


static void*  Thread::start_thread(void *obj)
{
  reinterpret_cast<Thread*>(obj)->run();
}




Thread::Thread() : m_stoprequested(false), m_running(false)
{
  pthread_mutex_init(&m_mutex, 0);
}

Thread::~Thread()
{
  pthread_mutex_destroy(&m_mutex);
}

/* Start the thread. */
void Thread::start()
{
  assert(m_running == false);
  m_running = true;
  pthread_create(&m_thread, 0, &Thread::start_thread, this);
}

/* Stop the thread. */
void Thread::stop()
{
  assert(m_running == true);
  m_running = false;
  m_stoprequested = true;
  pthread_join(m_thread, 0);
}
