

#ifndef SERVER4_H_
#define SERVER4_H_

#include "Server.h"
#include "Admin.h"
#include "Logger.h"
#include "Client.h"
#include "Socket.h"
#include <string>
#include <assert.h>
#include <pthread.h>
#include <vector>

using namespace std;

class Server4 {
public:

	// loaded from configuration file
	int port;
	string controlServerAddress;
	string identificationTag;
	vector<Admin*> administrators;

	bool connected;
	Server* parent;
	Logger* logger;
	pthread_t mainthread;

	vector<Client*> clients;
	vector<Server*> servers;

	enum STATUS {
			ADMIN = 0,
			REGULAR = 1
		};


	void addServer(string address, bool parent);
	void addClient(Client* client);

	Server4(){}
	~Server4(){}
};

#endif /* SERVER4_H_ */
