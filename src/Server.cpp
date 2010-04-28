#include "Server.h"
#include "Message.h"
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

	//if (server_socket.connectTo(server->getIpAddress(), server->getPort()) >= 0) {
	if (server_socket.connectTo("localhost", server->getPort()) >= 0) {
		std::cout << "made connection with parent" << std::endl;

		Message::MessageToSocket(&server_socket, &m);

		Message* response;
		while (true) {
			response = Message::messageFromSocket(&server_socket, true);
			response->parseData();

			std::cout << "From parent Server Message type: " << response->type << "\n\n";
			if (response->type == PING) {

				Message msg;
				msg.type = PONG;
				msg.buildRawData();

				Message::MessageToSocket(&server_socket, &msg);

			} else {
				//BREAKING CONNECTION   MSS 603 to CONTROL SERVER

				Message msg;
				msg.type = PEER_LOST;
				msg.addParameter(targetServerName);
				msg.buildRawData();

				Message::MessageToSocket(&controlSrv_socket, &msg);

				return 0;
			}
		}
	}
	return 0;
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

void Server::connectToParent(string ownSocketaddress){
	this->ownSocketaddress = ownSocketaddress;

	pthread_t server_thread;
	if (parent) {
		int error = pthread_create(&server_thread, 0, Server::parentServerThread, this);
	}
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

Server::~Server(){
	std::cout << "DESTROYING SEVER" << serverAddress[2] << std::endl;
}