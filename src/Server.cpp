#include "Server.h"
#include "Message.h"
#include "Socket.h"
#include "MessageType.h"
#include <iostream>
#include <cstdlib>

void* Server::parentServerThread(void *_obj) {

	Server* server = (Server*) _obj;
	std::cout << "Connecting to parent: " << server->getIpAddress() << ":" << server->getPort() << "\n\n";

	Message m;
	m.type = SERVER_REGISTER;
	m.addParameter(server->ownSocketaddress);
	m.buildRawData();

	Socket server_socket;

	if (server_socket.connectTo(server->getIpAddress(), server->getPort()) >= 0) {
		std::cout << "made connection with parent" << std::endl;

		Message::MessageToSocket(&server_socket, &m);

		Message* response;
		while (true) {
			response = Message::messageFromSocket(&server_socket);
			response->parseData();

			std::cout << "From parent Server Message type: " << response->type << "\n\n";
		}
	}
	return 0;
}

Server::Server(string socketaddress, string ownSocketaddress, bool parent) {

	this->ownSocketaddress = ownSocketaddress;
	int counter = 0;

	for (unsigned int i = 0; i < socketaddress.length(); i++) {

		if (socketaddress.at(i) != ':') {
			this->serverAddress[counter].append(1, socketaddress.at(i));

		} else {
			counter++;
		}
	}
	std::cout << "serverAddress: " << serverAddress[0] << "\n\n";

	pthread_t server_thread;
	if (parent) {
		int error = pthread_create(&server_thread, 0, Server::parentServerThread, this);
	}
}

string Server::getIpAddress() {
	return serverAddress[0];
}

int Server::getPort() {
	return std::atoi(serverAddress[1].c_str());
}
