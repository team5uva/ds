#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>
#include "Socket.h"
class Server4;

using namespace std;

class Server {
private:
	Server* parent;
	string serverAddress[3];
	
public:
	Server(string name);
	~Server();

	Server4* server4;
	string ownSocketaddress;
	string targetServerName;

	void connectToParent(string);
	string getIpAddress();
	string getTag();
	int getPort();
	static void* parentServerThread(void *_obj);
	void setControlSocket(Socket &socket);
	void messageToControl(int mssType, string parameter);
};

#endif /* SERVER_H_ */
