#include <sstream>
#include <string>
#include "ServerServerTest.h"
#include "Thread.h"
#include "MessageType.h"
#include "Server.h"

CPPUNIT_TEST_SUITE_REGISTRATION (ServerServerTest);

void ServerServerTest :: setUp (void)
{
    // set up test environment (initializing objects)
    a = new Server4();
    b = new Server4();

    Client* bert = new Client();
    bert->name = "bert";
    Client* ernie = new Client();
    ernie->name = "ernie";

    a->addClient(bert);
    b->addClient(ernie);

    
}

void ServerServerTest :: tearDown (void)
{
    // finally delete objects
    delete a; delete b;
}

void ServerServerTest :: transmissionTest (void)
{

    Socket* listenSocket = new Socket;
    int port = 2001;
    port = listenSocket->bindTo(port);
    listenSocket->listenForConn();

    Thread* threadA = new Thread();
    Thread* threadB = new Thread();

    string addressA = "127.0.0.1:";
    std::stringstream out;
    out << port;
    addressA.append(out.str());
    addressA.append(":serverA");
    string addressB = "127.0.0.1:1234:serverB";

    Server serverAinB(addressA);
    serverAinB.server4 = b;
    serverAinB.connectToParent(addressB);

    Socket* socketA = listenSocket->acceptConn();
    threadA->start(socketA, a);

    int matches = 0;
    for (int i = 0; i < a->clients.size(); i++)
    {
      for (int j = 0; j < b->clients.size(); j++)
      {
	if (a->clients[i] == b->clients[j])
	  matches++;
      }
    }

    usleep(30 * 1000000);

    CPPUNIT_ASSERT_EQUAL (matches, (int)a->clients.size());
    CPPUNIT_ASSERT_EQUAL (matches, (int)b->clients.size());

}

