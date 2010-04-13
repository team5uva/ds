#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <vector>
#include "Socket.h"
using namespace std;

class Message {
private:
  unsigned char* rawData;
  int length;

  vector<string> words;
  
  string message;
  int type;

public:
	static Message* messageFromSocket(Socket* s);
	Message(unsigned char* data, int length);
	void parseData();
	int getType();
	~Message();
};

#endif /* MESSAGE_H_ */
