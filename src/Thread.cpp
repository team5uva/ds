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
  reinterpret_cast<Thread*> (obj)->determineType();
}

void Thread::runClient(Client* c) {

    Message* receivedMessage;
    Message response;
    bool sleep;

    lastActivityTime = time(0);
    waiting_for_pong = false;

    cout << "Started client thread." << endl;

    // Send registration success message to connecting client
    response.type = REGISTRATION_SUCCESS;
    response.buildRawData();
    Message::MessageToSocket(socket, &response);

    /* Send client name to client. */
    processClientBroadcast(c, latestBroadcast);

    // Send all other client names to connecting client
    pthread_mutex_lock(&(server4->m_clients));
    for (int i = 0; i < server4->clients.size(); i++) {
      if (server4->clients[i] == c)
        continue;
      Message clientNameMsg;
      clientNameMsg.type = CLIENT_ADDED;
      clientNameMsg.addParameter(server4->clients[i]->name);
      clientNameMsg.buildRawData();
      Message::MessageToSocket(socket, &clientNameMsg);
    }
    pthread_mutex_unlock(&(server4->m_clients));

    while (!m_stoprequested) {
      sleep = true;

      if (lastActivityTime + 10 < time(0) && !waiting_for_pong)
        ping();
      else if (lastActivityTime + 2 < time(0) && waiting_for_pong)
      {
	std::cout << "ending connection, no pong in correct time" << std::endl;
	stop(true);
      }
      else
      {


	receivedMessage = Message::messageFromSocket(socket, false);

	if (receivedMessage != NULL) {
	  cout << "from client: " << c->name << endl;
	  receivedMessage->parseData();
	  cout << "received message with type: " << receivedMessage->getType() << endl;

	  processClientMessage(c, receivedMessage);

	  sleep = false;
	}

	else if (latestBroadcast == NULL)
	  latestBroadcast = server4->getLatestBroadcast();
	else if (latestBroadcast->next != NULL) {
	  latestBroadcast = latestBroadcast->next;
	  processClientBroadcast(c, latestBroadcast);
	  sleep = false;
	}
      }

      if (sleep)
        usleep(50000);
      else
        lastActivityTime = time(0);
    }
}

void Thread::processClientMessage(Client* c, Message* msg) {
  Message response;

  if (msg->type == CLIENT_REMOVED_FROM_CLIENT) {
    msg->words.insert(msg->words.begin(), c->name);
    server4->addBroadcast(CLIENT_REMOVED_FROM_SERVER, &(msg->words));

    pthread_mutex_lock(&(server4->m_clients));
    for (int i = 0; i < server4->clients.size(); i++)
      if (server4->clients[i] == c)
        server4->clients.erase(server4->clients.begin() + i);
    pthread_mutex_unlock(&(server4->m_clients));

    stop(true);
  } else if (msg->type == PING) {
    response.type = PONG;
    response.addParameter(msg->words.at(0));
    response.buildRawData();
    Message::MessageToSocket(socket, &response);
  } else if (msg->type == NAMECHANGE_FROM_CLIENT) {
    /* If name already exists, name change fails. */
    for (int i = 0; i < server4->clients.size() && response.type != NAMECHANGE_FAIL; i++)
      if (server4->clients[i]->changedName.compare(msg->words[0]) == 0)
        response.type = NAMECHANGE_FAIL;

    /* If namechange is permitted, send success message and notify rest. */
    if (response.type != NAMECHANGE_FAIL) {
      response.type = NAMECHANGE_SUCCESS;

      msg->words.insert(msg->words.begin(), c->changedName);
      c->changedName = msg->words[1];
      server4->addBroadcast(NAMECHANGE_FROM_SERVER, &(msg->words));
    }

    response.buildRawData();
    Message::MessageToSocket(socket, &response);
  } else if (msg->type == TEXT_FROM_CLIENT) {
    msg->words.insert(msg->words.begin(), c->changedName);
    server4->addBroadcast(TEXT_FROM_SERVER, &(msg->words));
  } else if (msg->type == ACTION_FROM_CLIENT) {
    msg->words.insert(msg->words.begin(), c->changedName);
    server4->addBroadcast(ACTION_FROM_SERVER, &(msg->words));
  } else if (msg->type == PONG) {
    waiting_for_pong = false;
  } else if (msg->type == SERVER_STOP && c->isAdmin)
    exit(0);
}

void Thread::processClientBroadcast(Client* c, Message* msg) {
  if (msg->type == CLIENT_ADDED || msg->type == NAMECHANGE_FROM_SERVER)
    Message::MessageToSocket(socket, msg);
  else if((msg->type == TEXT_FROM_SERVER || msg->type == ACTION_FROM_SERVER) &&
      (msg->words[1].compare(c->changedName) == 0 ||
      msg->words[0].compare(c->changedName) == 0 ||
      msg->words[1].compare("#all") == 0))
    Message::MessageToSocket(socket, msg);
}

void Thread::runServer(Server* s) {
  Message* receivedMessage;
  Message response;
  bool sleep;

  lastActivityTime = time(0);
  cout << "Started server thread." << endl;

  //Connected clients given to new server
  pthread_mutex_lock(&(server4->m_clients));
  for (int i = 0; i < server4->clients.size(); i++) {
    Message clientNameMsg;
    clientNameMsg.type = CLIENT_ADDED;
    clientNameMsg.addParameter(server4->clients[i]->name);
    clientNameMsg.buildRawData();
    Message::MessageToSocket(socket, &clientNameMsg);
  }
  pthread_mutex_unlock(&(server4->m_clients));

  //Connected servers to new server
  while (!m_stoprequested)
  {
    sleep = true;

    if (lastActivityTime + 10 < time(0) && !waiting_for_pong)
      ping();
    else if (lastActivityTime + 2 < time(0) && waiting_for_pong)
    {
      std::cout << "ending connection, no pong in correct time" << std::endl;
      stop(false);
    }
    else
    {
      //ProcessServerMessage
      receivedMessage = Message::messageFromSocket(socket, false);

      if (receivedMessage != NULL) {
	cout << "from Server: " << s->getIpAddress() << endl;
	receivedMessage->parseData();
	cout << "received message with type: " << receivedMessage->getType() << endl;

	processServerMessage(s, receivedMessage);

	sleep = false;
      }

      //ProcessServer Broadcas
      else if (latestBroadcast == NULL)
	latestBroadcast = server4->getLatestBroadcast();
      else if (latestBroadcast->next != NULL) {
	latestBroadcast = latestBroadcast->next;
	processServerBroadcast(s, latestBroadcast);
	sleep = false;
      }
    }

    if (sleep)
      usleep(50000);
    else
      lastActivityTime = time(0);
  }
}

void Thread::processServerMessage(Server* s, Message* m) {
  /* Message types to handle:
     
   */
}

void Thread::processServerBroadcast(Server* s, Message* m) {

}

/* Start the thread. */
void Thread::start(Socket* socket, Server4* server4) {
  assert(m_running == false);
  m_running = true;
  this->socket = socket;
  this->server4 = server4;
  pthread_create(&m_thread, 0, &Thread::start_thread, this);
}

/* Stop the thread. */
void Thread::stop(bool isClient) {
  assert(m_running == true);
  m_running = false;
  m_stoprequested = true;
  delete socket;
  socket = 0;
  cout << "Stopping " << (isClient ? "client" : "server") << " thread." << endl;
  pthread_join(m_thread, 0);
}

void Thread::determineType() {
  Message* firstMessage = Message::messageFromSocket(socket, true);
  firstMessage->parseData();
  bool registered = false;

  if (firstMessage->getType() == CLIENT_REGISTER) {
    Client* c = new Client();

    do {
      c->name = c->changedName = firstMessage->words[0];
      cout << firstMessage->words[0] << endl;

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
      }

      if(!registered) {
        delete firstMessage;
        firstMessage = 0;
        firstMessage = Message::messageFromSocket(socket, true);
        firstMessage->parseData();
      }
    } while(!registered);

    this->server4->addClient(c);

    firstMessage->words.resize(1);
    latestBroadcast = server4->addBroadcast(CLIENT_ADDED, &(firstMessage->words));

    runClient(c);
  } else if (firstMessage->getType() == SERVER_REGISTER) {

    Server* s = new Server(firstMessage->words[0]);

    std::cout << "new Child Server: " << firstMessage->words[0] << "    -  " << firstMessage->words.size() << std::endl;
    this->server4->addServer(s, false);

    //server4->addBroadcast(CLIENT_ADDED, &(firstMessage->words));
    runServer(s);

  } else {
    std::cout << "invalid connection attempt" << std::endl;
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
