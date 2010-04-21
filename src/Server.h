#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>

using namespace std;

class Server {
private:
	Server* parent;
	string serverAddress[3];
	string ownSocketaddress;

public:
	Server(string socketaddress, string ownSocketaddress, bool parent);
	~Server();

	string getIpAddress();
	int getPort();
	static void* parentServerThread(void *_obj);
};

#endif /* SERVER_H_ */
