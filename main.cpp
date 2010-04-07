#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[])
{
	int s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) // did not get valid socket
	{
		std::cout << "failed to aquire socket" << std::endl;
		return 1;
	}

	struct sockaddr_in address = {};

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(2001); //TODO get port from config file
	if(bind(s, (struct sockaddr *) &address, sizeof(address))<0)
	{
		std::cout << "failed to bind port for socket" << std::endl;
		return 1;
	}

	listen(s, 5);

	struct sockaddr_in client_address;
	socklen_t client_length = sizeof(client_address);
	int client_socket;
	client_socket = accept(s, (struct sockaddr *) &client_address, 
			&client_length);

	close(s);
	close(client_socket);



	std::cout << "Hello World" << std::endl;

}
