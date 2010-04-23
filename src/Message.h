#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <vector>
class Socket;
class Message;

using namespace std;

class Message
{
public:
  int type;
  int length;
  vector<string> words;
  unsigned char* rawData;
  bool isMulticast;
  Message* next;

  static void MessageToSocket(Socket* s, Message* m);
  static Message* messageFromSocket(Socket* s, bool blocking);
  Message(unsigned char* data, int length);
  Message();
  void parseData();
  void buildRawData();
  int getType();
  void addParameter(string s);
  ~Message(){}
};

#endif /* MESSAGE_H_ */

