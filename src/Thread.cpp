#include "Thread.h"
#include <iostream>

using namespace std;

Thread::Thread() : m_stoprequested(false), m_running(false)
{
  pthread_mutex_init(&m_mutex, 0);
}

Thread::~Thread()
{
  pthread_mutex_destroy(&m_mutex);
}

void* Thread::start_thread(void *obj)
{
  reinterpret_cast<Thread*>(obj)->determineType();
}

void Thread::runClient()
{
  while (!m_stoprequested)
  {
    string message;
    unsigned char inBuffer[200];
    int msgLength, msgCode, inBufLength = 0;

    while(true)
    {
      inBufLength = socket->readBytes(inBuffer, inBufLength, 2);
      msgLength = ((inBuffer[0]) << 8) + (inBuffer[1]);

      if(msgLength == 0)
        break;

      inBufLength = socket->readBytes(inBuffer, inBufLength, 2);
      msgCode = ((inBuffer[0]) << 8) + (inBuffer[1]);

      if(msgLength - 4 > 0)
      {
        inBufLength = socket->readBytes(inBuffer, inBufLength, msgLength - 4);
        message = string((char*)inBuffer, msgLength - 4);
      }
      else
        message = string();

      //std::cout << "Message length: " << msgLength << std::endl;
      std::cout << "Message code: " << msgCode << std::endl;
      std::cout << "Message: " << message << std::endl;
    }
  }
}

void Thread::runServer()
{
}

/* Start the thread. */
void Thread::start(Socket* socket, vector<Thread*>* threads)
{
  assert(m_running == false);
  m_running = true;
  this->socket = socket;
  this->threads = threads;
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

void Thread::determineType()
{
   runClient();
}

int Thread::getType()
{
  return type;
}
