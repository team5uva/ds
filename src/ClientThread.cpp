#include <iostream>
#include <time.h>
#include <cstdlib>
#include "MessageType.h"
#include "Thread.h"
#include "Client.h"

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


	Message* response = new Message();
	response->type = CLIENT_REMOVED_FROM_SERVER;
	response->addParameter(c->name);
	response->addParameter("client timed out");
	response->buildRawData();
	server4->addBroadcast(response);

	pthread_mutex_lock(&(server4->m_clients));
	for (int i = 0; i < server4->clients.size(); i++)
	  if (server4->clients[i] == c)
	    server4->clients.erase(server4->clients.begin() + i);
	pthread_mutex_unlock(&(server4->m_clients));


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
  if (msg->type == CLIENT_ADDED || msg->type == NAMECHANGE_FROM_SERVER || CLIENT_REMOVED_FROM_SERVER)
    Message::MessageToSocket(socket, msg);
  else if((msg->type == TEXT_FROM_SERVER || msg->type == ACTION_FROM_SERVER) &&
      (msg->words[1].compare(c->changedName) == 0 ||
      msg->words[0].compare(c->changedName) == 0 ||
      msg->words[1].compare("#all") == 0))
    Message::MessageToSocket(socket, msg);
}