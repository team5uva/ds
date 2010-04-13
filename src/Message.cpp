#include <arpa/inet.h>
#include <assert.h>
#include <iostream>
#include "Message.h"
#include "MessageType.h"

Message* Message::messageFromSocket(Socket* s)
{
  unsigned char* message = new unsigned char[200];
  short length;
  s->peekBytes((unsigned char*)&length, 0, 2);
  length = htons(length);
  s->readBytes(message, 0, length);

  return new Message(message, length);
}

Message::Message(unsigned char* data, int length)
{
  this->rawData = data;
  this->length = length;
}

int Message::getType()
{
  return type;
}

void Message::parseData()
{
  //assert((rawData[0] << 8) + rawData[1]) == length);

  this->type = (rawData[2] << 8) + rawData[3];

  words.push_back(string());

  for (int i = 4; i < length; i++)
  {
    if(rawData[i] == ' ' || rawData[i] == '\t')
    {
      if(words.back().length() > 0)
	words.push_back(string());
    }
    else
      words.back().push_back(rawData[i]);
  }
}
