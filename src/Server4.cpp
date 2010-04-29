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
#include <pthread.h>
#include "Server4.h"
#include "Server.h"
#include "configFile.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "Admin.h"
#include "Thread.h"
#include "Message.h"
#include "Thread.h"
#include "MessageType.h"

using namespace std;

Socket controlServer_socket;

Server4::Server4() {

	configFile config;

	// Read configuration file
	if (config.parseFile() == configFile::SUCCESS) {
		pthread_mutex_init(&m_broadcast, 0);
		pthread_mutex_init(&m_clients, 0);

		this->administrators = config.adminAccess;

		// No problems detected in configuration file
		// Maybe some detection here to check if all variables have indeed been set
		cout << "No problems detected in configuration file." << endl;
		for (unsigned int i = 0; i < config.adminAccess.size(); i++) {
			cout << config.adminAccess.at(i)->name << endl;
			cout << config.adminAccess.at(i)->password << endl;
		}

		port = config.getListenPort();
		identificationTag = config.getTag();

		listenSocket = new Socket;
		port = listenSocket->bindTo(port);
		listenSocket->listenForConn();

		address.append("87.210.237.85:");
		stringstream out;
		out << port;
		address.append(out.str());
		address.append(":");
		address.append(identificationTag);

		pthread_t controlServer_thread;
		int error = pthread_create(&controlServer_thread, 0, Server4::controlThread, this);

	} else {
		// Error in configuration file, shut down program
		// Maybe make a breakdown of different sort of errors to handle it more gracefully
		cout << "Problem detected in configuration file, shutting down..." << endl;
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
	Message* result;
	pthread_mutex_lock(&m_broadcast);
	result = broadcastList.back();
	pthread_mutex_unlock(&m_broadcast);

	return result;
}

void Server4::addServer(Server* server, bool parent) {
	cout << "Adding server  ..." << "\n";

	servers.push_back(server);

	if (parent) {
		parentServer = server;
		parentServer->setControlSocket(controlServer_socket);
		parentServer->connectToParent(address);
	}
}

void Server4::deleteServer(string serverTag) {

	for (unsigned int i = 0; i < servers.size(); i++) {
		if (servers[i]->getTag() == serverTag) {

			cout << "deleting Server " << serverTag << "\n";

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
		response->parseData();

		cout << "From Control Server Message type: " << response->type << "\n";

		// Get adres of the parent server if exists
		if (response->type == ADDRESS_FROM_CONTROL) {

			if (response->words[0] != "none") {
				cout << "Got parent server address: " << response->words[0] << "\n\n";

				//Create parent server
				Server* server = new Server(response->words[0]);
				server4->addServer(server, true);

			} else {
				cout << "No parent server exists." "\n\n";
			}

			// PING - PONG control server
		} else if (response->type == PING) {
			Message msg;
			msg.type = PONG;
			msg.buildRawData();

			Message::MessageToSocket(&controlServer_socket, &msg);

		} else if (response->type == REGROUP) {

			cout << "REGROUP: " << response->words[0] << " new parent: " << response->words[1] << "\n\n";

			server4->deleteServer(response->words[0]);

			if (response->words[1] != "none") {
				cout << "Got parent server address: " << response->words[1] << "\n\n";

				//Create parent server
				Server* server = new Server(response->words[1]);
				server4->addServer(server, true);
			}

		} else {
			//BREAKING CONNECTION
			cout << "Lost connection with Control. Reconnecting... " << "\n\n";
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

	cout << "To Control Server Message: " << m.words[0] << "\n";

	controlServer_socket.connectTo("146.50.1.102", 2001);
	Message::MessageToSocket(&controlServer_socket, &m);

}

int main(int argc, char* argv[]) {
	Thread* thread;
	Client* all;
	Socket *clientSocket = new Socket;

	Server4 server4;

	all = new Client();
	all->name = all->changedName = "#all";
	server4.addClient(all);

	while (true) {
		clientSocket = server4.listenSocket->acceptConn();
		thread = new Thread();
		thread->start(clientSocket, &server4);
	}

	shutdown(clientSocket->getSockfd(), 1);

	close(clientSocket->getSockfd());
	std::cout << "Teh end." << std::endl;
}
