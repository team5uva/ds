#include "Thread.h"
#include "Message.h"
#include "Server.h"
#include "MessageType.h"
#include <iostream>
#include <time.h>
#include <cstdlib>

using namespace std;

Thread::Thread() :
m_stoprequested(false), m_running(false) {
  pthread_mutex_init(&m_mutex, 0);
}

Thread::~Thread() {
  pthread_mutex_destroy(&m_mutex);
}

void* Thread::start_thread(void *obj) {
  Thread* t = reinterpret_cast<Thread*>(obj);
  if (t->parentServer == NULL)
    t->determineType();
  else
    t->startParentConnection();
}

void Thread::start(Server* server, Server4* server4)
{
  assert(m_running == false);
  m_running = true;
  this->server4 = server4;
  this->parentServer = server;
  this->socket = NULL;
  pthread_create(&m_thread, 0, &Thread::start_thread, this);
}
/* Start the thread. */
void Thread::start(Socket* socket, Server4* server4) {
  assert(m_running == false);
  m_running = true;
  this->socket = socket;
  this->server4 = server4;
  this->parentServer = NULL;
  pthread_create(&m_thread, 0, &Thread::start_thread, this);
}

/* Stop the thread. */
void Thread::stop(bool isClient) {
  assert(m_running == true);
  m_stoprequested = true;
  while (m_running && !isClient);
  delete socket;
  socket = 0;
  server4->logStream << "Stopping " << (isClient ? "client" : "server") << " thread." << endl;
  pthread_join(m_thread, 0);
}
void Thread::startParentConnection()
{
  Server* server = parentServer;
  server4->logStream << "Connecting to parent: " << server->getIpAddress() << ":" << server->getPort() << "\n\n";

  Message m;
  m.type = SERVER_REGISTER;
  m.addParameter(server->ownSocketaddress);
  m.buildRawData();

  Socket* server_socket = new Socket();
  socket = server_socket;

  if (server_socket->connectTo(server->getIpAddress(), server->getPort()) >= 0) {
    server4->logStream << "made connection with parent" << std::endl;
    Message::MessageToSocket(server_socket, &m);
    runServer(server);
  } else {
    server4->logStream << "The Parent Server is not responding. Send message PEER_LOST to Control... " << "\n\n";
    server->messageToControl(PEER_LOST, server->targetServerName);
  }
  return;
  

}
void Thread::determineType() {
  Message* firstMessage = Message::messageFromSocket(socket, true);
  if (firstMessage == NULL)
  {
    server4->logStream << "incoming connection with invalid first message, quiting" << endl;
    stop(false);
    return;
  }
  firstMessage->parseData();
  bool registered = false;

  if (firstMessage->getType() == CLIENT_REGISTER) {
    Client* c = new Client();

    do {
      c->name = firstMessage->words[0];

      /* If password was included */
      if (firstMessage->words.size() > 1) {
        c->password = firstMessage->words[1];
        /* Check password validity here. */
        for(int i = 0; i < this->server4->administrators.size(); i++) {
          if(c->name.compare(this->server4->administrators[i]->name) == 0 &&
              c->password.compare(this->server4->administrators[i]->password) == 0)
          {
            c->isAdmin = true;
            registered = true;
          }
        }

        if(!registered) {
          Message response;
          response.type = REGISTRATION_FAIL;
          response.addParameter("Username/password combination unknown.");
          response.buildRawData();
          Message::MessageToSocket(socket, &response);
        }
      }
      else
      {
        c->isAdmin = false;
        registered = true;

	for (int i = 0; i < server4->clients.size(); i++)
	  if (server4->clients[i]->name.compare(c->name) == 0)
	    registered = false;

	if (!registered)
	{
          Message response;
          response.type = REGISTRATION_FAIL;
          response.addParameter("Username already in use on server.");
          response.buildRawData();
          Message::MessageToSocket(socket, &response);
	}
      }

      if(!registered) {
        delete firstMessage;
        firstMessage = 0;
        firstMessage = Message::messageFromSocket(socket, true);
	if (firstMessage == NULL)
	{
	  server4->logStream << "incoming connection with invalid first message, quiting" << endl;
	  stop(false);
	  return;
	}
        firstMessage->parseData();
      }
    } while(!registered);

    this->server4->addClient(c);

    firstMessage->words.resize(1);
    latestBroadcast = server4->addBroadcast(CLIENT_ADDED, &(firstMessage->words));

    runClient(c);
  } else if (firstMessage->getType() == SERVER_REGISTER) {

    Server* s = new Server(firstMessage->words[0]);

    server4->logStream << "new Child Server: " << firstMessage->words[0] << "    -  " << firstMessage->words.size() << std::endl;
    this->server4->addServer(s, false);

    latestBroadcast = server4->addBroadcast(0, &(firstMessage->words));
    runServer(s);

  } else {
    server4->logStream << "invalid connection attempt" << std::endl;
  }
}

int Thread::getType() {
  return type;
}

void Thread::ping() {
  Message ping;
  ping.type = PING;
  ping.addParameter(server4->identificationTag);
  ping.buildRawData();
  Message::MessageToSocket(socket, &ping);
  lastActivityTime = time(0);
  waiting_for_pong = true;
}
