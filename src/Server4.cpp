/* The main program. As of now it waits for a client to connect on the specified
 * port. Once a client connects, all the incoming messages are displayed.
 * Usage (for now):
 * ./executable <listenport>
 */
#include <sstream>
#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include "Server4.h"
#include "Server.h"
#include "configFile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include "Admin.h"
#include "Thread.h"
#include "Message.h"
#include "Thread.h"
#include "MessageType.h"
#include <time.h>

using namespace std;

Socket controlServer_socket;

Server4::Server4() {


	// Read configuration file
	if (config.parseFile() == configFile::SUCCESS) {
		pthread_mutex_init(&m_broadcast, 0);
		pthread_mutex_init(&m_clients, 0);

		this->administrators = config.adminAccess;


		port = config.getListenPort();
		identificationTag = config.getTag();
		std::ostringstream oss;
		srandom(time(0));
		int extraBytes = 32 - identificationTag.length();
		for (int i = 0; i < extraBytes; i++)
		{
		  oss << (random()%10);
		}
		identificationTag.append(oss.str());

		char buf[100];
		gethostname(buf, 100);
		struct hostent *host_entry;
	        host_entry = gethostbyname(buf);
		address.append(inet_ntoa(*((struct in_addr *)host_entry->h_addr_list[0])));
		address.append(":");
		stringstream out;
		out << port;
		address.append(out.str());
		string logfile = "logfile.";
		if (config.getPerIpLog())
		{
		  logfile.append(address);
		  logfile.append(".");
		}
		logfile.append("log");
		logStream.open(logfile.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
		address.append(":");
		address.append(identificationTag);

		//pthread_t controlServer_thread;
		//int error = pthread_create(&controlServer_thread, 0, Server4::controlThread, this);
                
		// No problems detected in configuration file
		// Maybe some detection here to check if all variables have indeed been set
		logStream << "No problems detected in configuration file." << endl;


	} else {
		// Error in configuration file, shut down program
		// Maybe make a breakdown of different sort of errors to handle it more gracefully
		logStream << "Problem detected in configuration file, shutting down..." << endl;
		exit(0);
	}
}

void Server4::addClient(Client* client) {
	pthread_mutex_lock(&m_clients);
	this->clients.push_back(client);
	pthread_mutex_unlock(&m_clients);
}

Message* Server4::addBroadcast(Message* msg) {
	pthread_mutex_lock(&m_broadcast);

	if (broadcastList.size() != 0)
		broadcastList.back()->next = msg;
	broadcastList.push_back(msg);

	/* Might need to delete/destroy the message as well, this needs to be looked at */
	if (broadcastList.size() > 100)
		broadcastList.erase(broadcastList.begin());

	pthread_mutex_unlock(&m_broadcast);

	return msg;
}

Message* Server4::addBroadcast(int type, vector<string>* words) {
	Message* msg = new Message;

	msg->type = type;

	for (int i = 0; i < words->size(); i++)
		msg->addParameter(words->at(i));
	msg->buildRawData();
	return addBroadcast(msg);
}

Message* Server4::getLatestBroadcast() {
        if (broadcastList.empty())
	  return NULL;

	Message* result;
	pthread_mutex_lock(&m_broadcast);
	result = broadcastList.back();
	pthread_mutex_unlock(&m_broadcast);

	return result;
}

void Server4::addServer(Server* server, bool parent) {
	logStream << "Adding server  ..." << "\n";

	servers.push_back(server);

	if (parent) {
		parentServer = server;
		parentServer->setControlSocket(controlServer_socket);
		//parentServer->connectToParent(address);
		server->ownSocketaddress = address;
		Thread* t = new Thread();
		t->start(server, this);

	}
}

void Server4::deleteServer(string serverTag) {

	for (unsigned int i = 0; i < servers.size(); i++) {
		if (servers[i]->getTag() == serverTag) {

			logStream << "deleting Server " << serverTag << "\n";

			delete servers[i];
			servers.erase(servers.begin() + i);
		}
	}
}

void* Server4::controlThread(void *_obj) {
	Server4* server4 = (Server4*) _obj;

	server4->connectToControl(server4);

	Message* response;
	while (true) {
		response = Message::messageFromSocket(&controlServer_socket, true);
		if(response == NULL)
			return NULL;

		response->parseData();

		server4->logStream << "From Control Server Message type: " << response->type << std::endl;

		// Get adres of the parent server if exists
		if (response->type == ADDRESS_FROM_CONTROL) {

			if (response->words[0] != "none") {
				server4->logStream << "Got parent server address: " << response->words[0] << std::endl << std::endl;

				//Create parent server
				Server* server = new Server(response->words[0]);
				server->server4 = server4;
				server4->addServer(server, true);

			} else {
				server4->logStream << "No parent server exists." << std::endl << std::endl;
			}

			// PING - PONG control server
		} else if (response->type == PING) {
			Message msg;
			msg.type = PONG;
			msg.addParameter(response->words[0]);
			msg.buildRawData();

			Message::MessageToSocket(&controlServer_socket, &msg);

		} else if (response->type == REGROUP) {
		        if (response->words.size() == 1)
			  response->words.push_back("none");

			server4->logStream << "REGROUP: " << response->words[0] << " new parent: " << response->words[1] << std::endl << std::endl;

			server4->deleteServer(response->words[0]);

			if (response->words[1] != "none" || response->words[1] == "") {
				server4->logStream << "Got parent server address: " << response->words[1] << std::endl << std::endl;

				//Create parent server
				Server* server = new Server(response->words[1]);
				server4->addServer(server, true);
			}

		} else {
			//BREAKING CONNECTION
			server4->logStream << "Lost connection with Control. Reconnecting... " << std::endl << std::endl;
			server4->connectToControl(server4);
		}
	}
	return NULL;
}

void Server4::connectToControl(Server4* server4) {
	Message m;
	m.type = ADDRESS_TO_CONTROL;
	m.addParameter(server4->address);

	m.buildRawData();

	logStream << "To Control Server Message: " << m.words[0] << std::endl;
	controlServer_socket.connectTo(server4->config.getCSAddress(), server4->config.getCSPort());
	Message::MessageToSocket(&controlServer_socket, &m);

}

