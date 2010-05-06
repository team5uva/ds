#ifndef SERVER4_H_
#define SERVER4_H_

#include "Admin.h"
#include "Logger.h"
#include "Client.h"
#include "Socket.h"
#include <string>
#include <assert.h>
#include <pthread.h>
#include <fstream>
#include <vector>

using namespace std;
class Server;

class Server4 {
public:

	// loaded from configuration file
	int port;
	string controlServerAddress;
	string identificationTag;
	string address;
	vector<Admin*> administrators;

	bool connected;
	Server* parentServer;
	Logger* logger;
	pthread_t mainthread;

	vector<Client*> clients;
	vector<Server*> servers;
	vector<Message*> broadcastList;
	pthread_mutex_t m_broadcast, m_clients;
	Socket* listenSocket;

	std::fstream logStream;

	enum STATUS {
		ADMIN = 0, REGULAR = 1
	};

	void addServer(Server* server, bool parent);
	void addClient(Client* client);
	Message* addBroadcast(Message* msg);
	Message* addBroadcast(int type, vector<string>* words);
	Message* getLatestBroadcast();
	void deleteServer(string serverTag);
	void connectToControl(Server4* server4);

	static void* controlThread(void *_obj);

	Server4();
	~Server4() {
	}
};

#endif /* SERVER4_H_ */
