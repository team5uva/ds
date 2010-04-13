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

void Message::MessageToSocket(Socket* s, Message* m)
{
  std::cout << "trying to send message of length " << m->length << " and code " << m->type << std::endl;
  s->writeBytes(m->rawData, m->length);
}

Message::Message(unsigned char* data, int length)
{
  this->rawData = data;
  this->length = length;
}

Message::Message()
{
  this->rawData = new unsigned char[200];
  this->length = 0;
}

int Message::getType()
{
  return type;
}

void Message::addParameter(string s)
{
  words.push_back(s);
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

void Message::buildRawData()
{
  short rawType = htons(type);
  rawData[2] = ((unsigned char*)&rawType)[0];
  rawData[3] = ((unsigned char*)&rawType)[1];


  string message;

  for (int i = 0; i < words.size(); i++)
  {
    message.append(words[i]);
    if (i + 1 < words.size())
      message.append(" ");
  }

  for (int i = 0; i < message.size(); i++)
  {
    rawData[i+4] = message[i];
  }

  length = 4 + message.size();
  short rawSize = htons(length);
  rawData[0] = rawSize;


  rawData[0] = ((unsigned char*)&rawSize)[0];
  rawData[1] = ((unsigned char*)&rawSize)[1];
}
