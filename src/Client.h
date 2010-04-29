
#ifndef CLIENT_H_
#define CLIENT_H_

#include "Server4.h"
#include <string>

using namespace std;

class Client {
public:

	string socketaddress;
	string name;
  string changedName;
	string password;
	Server* parentServer;
  bool isAdmin;

	Client(){}
	~Client(){}
};

#endif /* CLIENT_H_ */
