

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
  vector<Message*> multicastList;
  pthread_mutex_t m_multicast;

	enum STATUS
  {
    ADMIN = 0,
    REGULAR = 1
	};

	void addServer(string address, bool parent);
	void addClient(Client* client);
  void addMulticast(Message* msg);
  void addMulticast(int type, vector<string>* words);
  Message* getLatestMulticast();

	Server4(){}
	~Server4(){}
};

#endif /* SERVER4_H_ */
