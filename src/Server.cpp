#include "Server.h"
#include "Server4.h"
#include "Message.h"
#include "Client.h"
#include "Socket.h"
#include "MessageType.h"
#include <iostream>
#include <cstdlib>

Socket controlSrv_socket;

/* Send a message to the control server. */
void Server::messageToControl(int mssType, string parameter) {
  Message msg;
  msg.type = mssType;
  msg.addParameter(parameter);
  msg.buildRawData();

  Message::MessageToSocket(&controlSrv_socket, &msg);
}

/*Server constructor. */
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

/* Gets the server's IP address. */
string Server::getIpAddress() {
  return serverAddress[0];
}

/* Gets the server's tag. */
string Server::getTag() {
  return serverAddress[2];
}

/* Gets the server's port. */
int Server::getPort() {
  return std::atoi(serverAddress[1].c_str());
}

/* Sets the control server socket. */
void Server::setControlSocket(Socket &socket) {
  controlSrv_socket = socket;
}

/* Message destructor. */
Server::~Server() {
}
