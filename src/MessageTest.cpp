#include "MessageTest.h"
#include "Socket.h"

CPPUNIT_TEST_SUITE_REGISTRATION (MessageTest);

void MessageTest :: setUp (void)
{
    // set up test environment (initializing objects)
    a = new Message();
    a->type = 10;
    a->addParameter("test");
}

void MessageTest :: tearDown (void)
{
    // finally delete objects
    delete a; delete b;
}

void MessageTest :: LocalParseMessageTest (void)
{
    a->buildRawData();
    b = new Message(a->rawData, a->length);
    b->parseData();

    // check equivalance results
    CPPUNIT_ASSERT_EQUAL (a->type, b->type);
    CPPUNIT_ASSERT_EQUAL (a->length, b->length);
    for(int i = 0; i < a->words.size(); i++) {
      CPPUNIT_ASSERT_EQUAL (a->words[i], b->words[i]);   
    }
}

void MessageTest :: SocketParseMessageTest(void)
{
    a->buildRawData();
    Socket* listenSocket = new Socket();
    int port = 2001;
    port = listenSocket->bindTo(port);
    listenSocket->listenForConn();

    Socket* toSocketA = new Socket();
    toSocketA->connectTo("127.0.1.1", port);

    Socket* socketA = listenSocket->acceptConn();

    Message::MessageToSocket(toSocketA, a);
    b = Message::messageFromSocket(socketA, true);
    b->parseData();

    
    // check equivalance results
    CPPUNIT_ASSERT_EQUAL (a->type, b->type);
    CPPUNIT_ASSERT_EQUAL (a->length, b->length);
    for(int i = 0; i < a->words.size(); i++) {
      CPPUNIT_ASSERT_EQUAL (a->words[i], b->words[i]);   
    }

}

