#include <arpa/inet.h>
#include <assert.h>
#include <iostream>
#include "Message.h"
#include "Socket.h"
#include "MessageType.h"

/* Gets a message from a given socket, the blocking variable decides
 * whether the operation is blocking or not.
 */
Message* Message::messageFromSocket(Socket* s, bool blocking) {
  unsigned char* message = new unsigned char[200];
  int bytesPeeked;
  short length;

  bytesPeeked = s->peekBytes((unsigned char*) & length, 0, 2, blocking);

  if (!blocking && bytesPeeked < 0)
    return NULL;

  if (bytesPeeked == 0)
    return NULL;

  length = htons(length);
  int readBytes = s->readBytes(message, 0, length);
  if (readBytes != length)
    return NULL;

  return new Message(message, length);
}

/* Sends a message to a socket. */
void Message::MessageToSocket(Socket* s, Message* m) {
  s->writeBytes(m->rawData, m->length);
}

/* Message constructor. */
Message::Message(unsigned char* data, int length) {
  this->rawData = data;
  this->length = length;
  origin = NULL;
  next = NULL;
}

/* Message constructor. */
Message::Message() {
  this->rawData = new unsigned char[200];
  this->length = 0;
  origin = NULL;
  next = NULL;
}

/* Gets the mssage type. */
int Message::getType() {
  return type;
}

/* Adds a word to the message. */
void Message::addParameter(string s) {
  words.push_back(s);
}

/* Parses the message data. */
void Message::parseData() {
  this->type = (rawData[2] << 8) + rawData[3];

  words.push_back(string());

  for (int i = 4; i < length; i++) {
    if (rawData[i] == ' ' || rawData[i] == '\t') {
      if (words.back().length() > 0)
        words.push_back(string());
    } else
      words.back().push_back(rawData[i]);
  }
}

/* Prepares message for transmission. */
void Message::buildRawData() {
  short rawType = htons(type);
  rawData[2] = ((unsigned char*) & rawType)[0];
  rawData[3] = ((unsigned char*) & rawType)[1];


  string message;

  for (int i = 0; i < words.size(); i++) {
    message.append(words[i]);
    if (i + 1 < words.size())
      message.append(" ");
  }

  for (int i = 0; i < message.size(); i++) {
    rawData[i + 4] = message[i];
  }

  length = 4 + message.size();
  short rawSize = htons(length);
  rawData[0] = rawSize;


  rawData[0] = ((unsigned char*) & rawSize)[0];
  rawData[1] = ((unsigned char*) & rawSize)[1];
}
