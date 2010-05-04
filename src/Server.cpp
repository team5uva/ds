#include "Server.h"
#include "Server4.h"
#include "Message.h"
#include "Client.h"
#include "Socket.h"
#include "MessageType.h"
#include <iostream>
#include <cstdlib>

Socket controlSrv_socket;
string targetServerName;

void* Server::parentServerThread(void *_obj) {

	Server* server = (Server*) _obj;
	std::cout << "Connecting to parent: " << server->getIpAddress() << ":" << server->getPort() << "\n\n";

	Message m;
	m.type = SERVER_REGISTER;
	m.addParameter(server->ownSocketaddress);
	m.buildRawData();

	Socket server_socket;

	if (server_socket.connectTo(server->getIpAddress(), server->getPort()) >= 0) {
	//if (server_socket.connectTo("localhost", server->getPort()) >= 0) {
		std::cout << "made connection with parent" << std::endl;

		Message::MessageToSocket(&server_socket, &m);

		Message* response;
		while (true) {
		        usleep(100000);
		        
		        if (response == NULL)
			  continue;
			response = Message::messageFromSocket(&server_socket, true);
			response->parseData();

			std::cout << "From parent Server Message type: " << response->type << "\n\n";
			if (response->type == PING) {

				Message msg;
				msg.type = PONG;
				msg.buildRawData();

				Message::MessageToSocket(&server_socket, &msg);

			} else if(response->type == CLIENT_ADDED) {
			        Client* c = new Client();
			        c->name = response->words[0];
			        c->parentServer = server;
			        c->isAdmin = false;
			        
			        server->server4->addClient(c);
			        server->server4->addBroadcast(response);
			} else {
				//BREAKING CONNECTION   MSS 603 to CONTROL SERVER
				server->messageToControl(PEER_LOST, targetServerName);
				return 0;
			}
			
		}
	} else {
		std::cout << "The Parent Server is not responding. Send message PEER_LOST to Control... " << "\n\n";
		server->messageToControl(PEER_LOST, targetServerName);
	}
	return 0;
}

void Server::messageToControl(int mssType, string parameter) {
	Message msg;
	msg.type = mssType;
	msg.addParameter(parameter);
	msg.buildRawData();

	Message::MessageToSocket(&controlSrv_socket, &msg);
}

Server::Server(string name) {

	targetServerName = name;
	int counter = 0;

	for (unsigned int i = 0; i < targetServerName.length(); i++) {

		if (targetServerName.at(i) != ':') {
			this->serverAddress[counter].append(1, targetServerName.at(i));

		} else {
			counter++;
		}
	}
}

void Server::connectToParent(string ownSocketaddress) {
	this->ownSocketaddress = ownSocketaddress;

	pthread_t server_thread;
	int error = pthread_create(&server_thread, 0, Server::parentServerThread, this);
}

string Server::getIpAddress() {
	return serverAddress[0];
}

string Server::getTag() {
	return serverAddress[2];
}

int Server::getPort() {
	return std::atoi(serverAddress[1].c_str());
}

void Server::setControlSocket(Socket &socket) {
	controlSrv_socket = socket;
}

Server::~Server() {
}
