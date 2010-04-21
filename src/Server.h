
#ifndef SERVER_H_
#define SERVER_H_

#include <string>
#include <vector>

using namespace std;

class Server {
private:
	Server* parent;

public:
	string socketaddress;

	Server();
	~Server();
};

#endif /* SERVER_H_ */
