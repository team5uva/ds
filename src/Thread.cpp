#include "Thread.h"
#include "Message.h"
#include "MessageType.h"
#include <iostream>
#include <time.h>

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

void Thread::runClient(Client* c)
{
  while (!m_stoprequested)
  {
    Message* receivedMessage, * latestMulticast;
    Message response;
    bool dontSleep;

    lastPingTime = time(0);

    cout << "started Client Thread" << endl;

    latestMulticast = server4->getLatestMulticast();

    // Send registration success message to connecting client
    response.type = REGISTRATION_SUCCESS;
    response.buildRawData();
    Message::MessageToSocket(socket,&response);

    // Send all other client names to connecting client
    for (int i = 0; i < server4->clients.size(); i++)
    {
      if (server4->clients[i] == c)
	      continue;
      Message clientNameMsg;
      clientNameMsg.type = CLIENT_ADDED;
      clientNameMsg.addParameter(server4->clients[i]->name);
      clientNameMsg.buildRawData();
      Message::MessageToSocket(socket, &clientNameMsg);
    }

    while(true)
    {
      dontSleep = false;
      
      if(lastPingTime + 10 < time(0))
        ping();
      
      receivedMessage = Message::messageFromSocket(socket, false);

      if(receivedMessage != NULL)
      {
        receivedMessage->parseData();
        processClientMessage(c, receivedMessage);
        dontSleep = true;
      }

      if(latestMulticast == NULL)
        latestMulticast = server4->getLatestMulticast();
      else if(latestMulticast->next != NULL)
      {
        latestMulticast = latestMulticast->next;
        processClientMulticast(latestMulticast);
        dontSleep = true;
      }

      if(!dontSleep)
        usleep(50000);
    }
  }
}

void Thread::processClientMessage(Client* c, Message* msg)
{
  Message response;

  if(msg->type == CLIENT_REMOVED_FROM_CLIENT)
  {
    msg->words.insert(msg->words.begin(), c->name);
    server4->addMulticast(CLIENT_REMOVED_FROM_SERVER, &(msg->words));

    for(int i = 0; i < server4->clients.size(); i++)
      if(server4->clients[i] == c)
        server4->clients.erase(server4->clients.begin() + i);
    
    stop();
  }
  else if(msg->type == PING)
  {
    response.type = PONG;
    response.addParameter(msg->words.at(0));
    response.buildRawData();
    Message::MessageToSocket(socket, &response);
  }
  else if(msg->type == NAMECHANGE_FROM_CLIENT)
  {
    /* If name already exists, name change fails. */
    for(int i = 0; i < server4->clients.size() && response.type == 0; i++)
      if(server4->clients[i]->name.compare(msg->words[0]) == 0)
        response.type = NAMECHANGE_FAIL;

    if(response.type != NAMECHANGE_FAIL)
    {
      response.type = NAMECHANGE_SUCCESS;

      msg->words.insert(msg->words.begin(), c->name);
      server4->addMulticast(NAMECHANGE_FROM_SERVER, &(msg->words));
    }

    response.buildRawData();
    Message::MessageToSocket(socket, &response);
  }
  else if(msg->type == TEXT_FROM_CLIENT)
  {
    msg->words.insert(msg->words.begin(), c->name);
    server4->addMulticast(TEXT_FROM_SERVER, &(msg->words));
  }
  else if(msg->type == ACTION_FROM_CLIENT)
  {
    msg->words.insert(msg->words.begin(), c->name);
    server4->addMulticast(ACTION_FROM_SERVER, &(msg->words));
  }
  else if(msg->type == PONG)
  {
    if(lastPingTime + 2 < time(0))
      stop();
  }
  else if(msg->type == SERVER_STOP)
    exit(0);
}

void Thread::processClientMulticast(Message* msg)
{
  if(msg->type == CLIENT_ADDED || msg->type == TEXT_FROM_SERVER ||
      msg->type == ACTION_FROM_SERVER || NAMECHANGE_FROM_SERVER)
    Message::MessageToSocket(socket, msg);
}

void Thread::runServer()
{
}

/* Start the thread. */
void Thread::start(Socket* socket, vector<Thread*>* threads, Server4* server4)
{
  assert(m_running == false);
  m_running = true;
  this->socket = socket;
  this->threads = threads;
  this->server4 = server4;
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
  Message* firstMessage = Message::messageFromSocket(socket, true);
  firstMessage->parseData();
  if(firstMessage->getType() == CLIENT_REGISTER)
  {
    Client* c = new Client();
    c->name = firstMessage->words[0];
    if (firstMessage->words.size() > 1)
      c->password = firstMessage->words[1];
    this->server4->addClient(c);
    
    server4->addMulticast(CLIENT_ADDED, &(firstMessage->words));

    runClient(c);
  }
  else if (firstMessage->getType() == SERVER_REGISTER)
  {
    runServer();
  }
  else
  {
    std::cout << "invalid connection attempt" << std::endl;
  }
}

int Thread::getType()
{
  return type;
}

void Thread::ping()
{
  Message ping;
  ping.type = PING;
  ping.addParameter(server4->identificationTag);
  ping.buildRawData();
  Message::MessageToSocket(socket, &ping);
  lastPingTime = time(0);
}
