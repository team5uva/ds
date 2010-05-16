#include <iostream>
#include <time.h>
#include <cstdlib>
#include "MessageType.h"
#include "Thread.h"
#include "Server.h"

/* Runs the server thread. */
void Thread::runServer(Server* s) {
  Message* receivedMessage;
  bool sleep;

  lastActivityTime = time(0);
  waiting_for_pong = false;
  latestBroadcast = NULL;

  server4->logStream << "Started server thread." << endl;

  /* Send list of connected clients to server. */
  pthread_mutex_lock(&(server4->m_clients));
  for (int i = 0; i < server4->clients.size(); i++) {
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
    /* If no pong is received after ping within two seconds, remove all clients
     * connected to this server from the global list of clients.
     */
    else if (lastActivityTime + 2 < time(0) && waiting_for_pong) {
      server4->logStream << "ending connection, no pong in correct time" << std::endl;
      pthread_mutex_lock(&(server4->m_clients));
      for (int i = 0; i < server4->clients.size(); i++) {
        if (server4->clients[i]->parentServer == s) {
          Message* response = new Message();
          response->type = CLIENT_REMOVED_FROM_SERVER;
          response->addParameter(server4->clients[i]->name);
          response->origin = s;
          server4->addBroadcast(response);
          server4->clients.erase(server4->clients.begin() + i);
        }
      }
      pthread_mutex_unlock(&(server4->m_clients));
      stop(false);
    /* Else keep sending and receiving messages. */
    } else {
      receivedMessage = Message::messageFromSocket(socket, false);

      if (receivedMessage != NULL) {
        server4->logStream << "from Server: " << s->getIpAddress();
        receivedMessage->parseData();
        server4->logStream << " received message with type: " << receivedMessage->getType() << endl;

        processServerMessage(s, receivedMessage);

        sleep = false;
      }
      else if (latestBroadcast == NULL) {
        latestBroadcast = server4->getLatestBroadcast();
      } else if (latestBroadcast->next != NULL) {
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
  m_running = false;
}

/* Processs a message from server. */
void Thread::processServerMessage(Server* s, Message* m) {
  /* Message types to handle:
     300 - TEXT_FROM_SERVER
     310 - ACTION_FROM_SERVER
     110 - CLIENT_ADDED
     130 - CLIENT_REMOVED_FROM_SERVER
     140 - PING
     150 - PONG
     170 - NAMECHANGE_SERVER
     600 - SERVER_REGISTER
   */
  Message* response = new Message();

  if (m->type == PING) {
    response->type = PONG;
    response->addParameter(m->words.at(0));
    response->buildRawData();
    Message::MessageToSocket(socket, response);
  } else if (m->type == PONG) {
    waiting_for_pong = false;
  } else if (m->type == CLIENT_ADDED) {
    Client* c = new Client();
    c->name = c->name = m->words[0];
    c->isAdmin = false;
    c->parentServer = s;
    response->type = CLIENT_ADDED;
    response->words = m->words;
    response->origin = s;

    pthread_mutex_lock(&(server4->m_clients));
    for (int i = 0; i < server4->clients.size(); i++)
      if (server4->clients[i]->name == c->name) {
        pthread_mutex_unlock(&(server4->m_clients));
        return;
      }
    pthread_mutex_unlock(&(server4->m_clients));

    this->server4->addClient(c);
    server4->addBroadcast(response);
  } else if (m->type == CLIENT_REMOVED_FROM_SERVER) {

    pthread_mutex_lock(&(server4->m_clients));
    for (int i = 0; i < server4->clients.size(); i++)
      if (server4->clients[i]->name == m->words[0])
        server4->clients.erase(server4->clients.begin() + i);
    pthread_mutex_unlock(&(server4->m_clients));
    response->type = CLIENT_REMOVED_FROM_SERVER;
    response->words = m->words;
    response->origin = s;

    server4->addBroadcast(response);

  } else if (m->type == TEXT_FROM_SERVER) {
    response->type = TEXT_FROM_SERVER;
    response->words = m->words;
    response->origin = s;
    server4->addBroadcast(response);
  } else if (m->type == ACTION_FROM_SERVER) {
    response->type = ACTION_FROM_SERVER;
    response->words = m->words;
    response->origin = s;
    server4->addBroadcast(response);
  } else if (m->type == NAMECHANGE_FROM_SERVER) {
    response->type = NAMECHANGE_FROM_SERVER;
    response->words = m->words;
    pthread_mutex_lock(&(server4->m_clients));
    for (int i = 0; i < server4->clients.size(); i++)
      if (server4->clients[i]->name == m->words[0])
        server4->clients[i]->name = m->words[1];
    pthread_mutex_unlock(&(server4->m_clients));
    response->origin = s;
    server4->addBroadcast(response);
  }
}

/* Processes a unicast/multicast from the global broadcast list. */
void Thread::processServerBroadcast(Server* s, Message* m) {
  m->buildRawData();
  if (m->type == CLIENT_REMOVED_FROM_SERVER && m->origin != s) {
    Message::MessageToSocket(socket, m);
  } else if (m->type == CLIENT_ADDED && m->origin != s) {
    Message::MessageToSocket(socket, m);
  } else if (m->type == TEXT_FROM_SERVER && m->origin != s) {
    Message::MessageToSocket(socket, m);
  } else if (m->type == ACTION_FROM_SERVER && m->origin != s) {
    Message::MessageToSocket(socket, m);
  } else if (m->type == NAMECHANGE_FROM_SERVER && m->origin != s) {
    Message::MessageToSocket(socket, m);
  }
}

