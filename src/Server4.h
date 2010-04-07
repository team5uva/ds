

#ifndef SERVER4_H_
#define SERVER4_H_

#include "Server.h"
#include "Admin.h"
#include "Logger.h"
#include "Client.h"
#include <string>
#include <pthread.h>
#include <vector>

using namespace std;

class Server4 {
private:

	// loaded from configuration file
	int port;
	string controlServerAddress;
	string identificationTag;
	vector<Admin*> administrators;

	bool connected;
	Server* parent;
	//char[] buffer;
	Logger* logger;
	pthread_t mainthread;

	vector<Client*> clients;
	vector<Server*> servers;

	enum STATUS {
			ADMIN = 0,
			REGULAR = 1
		};

	enum MESSAGETYPE {
			REGISTERCLIENT = 100,
			CLIENTADDED = 110,
			CLIENTDELETED = 120 // en nog meer ...
		};


public:
	Server4();
	~Server4();
};

#endif /* SERVER4_H_ */
