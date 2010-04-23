#ifndef MESSAGE_H_
#define MESSAGE_H_
#include <string>
#include <vector>
class Socket;
class Message;

using namespace std;

class Message {
private:
public:
  int length;
  int type;
  vector<string> words;
  unsigned char* rawData;
  static void MessageToSocket(Socket* s, Message* m);
  static Message* messageFromSocket(Socket* s);
  Message(unsigned char* data, int length);
  Message();
  void parseData();
  void buildRawData();
  int getType();
  void addParameter(string s);
  ~Message(){}
};

#endif /* MESSAGE_H_ */
