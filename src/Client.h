
#ifndef CLIENT_H_
#define CLIENT_H_

#include "Server.h"
#include "Server4.h"
#include <string>

using namespace std;

class Client {
private:

	string socketaddress;
	string name;
	string password;
	Server* parentSrever;
	int authorizationstatus;

public:
	Client();
	~Client();
};

#endif /* CLIENT_H_ */
