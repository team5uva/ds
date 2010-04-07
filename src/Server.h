
#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>

using namespace std;

class Server {
private:

	string socketaddress;
	vector<Server*> route;

public:
	Server();
	~Server();
};

#endif /* SERVER_H_ */
