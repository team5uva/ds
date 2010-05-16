#include <sstream>
#include <string>
#include "ServerServerTest.h"
#include "Thread.h"
#include "MessageType.h"
#include "Message.h"
#include "Server.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ServerServerTest);

void ServerServerTest::setUp(void) {
  // set up test environment (initializing objects)
  a = new Server4();
  a->logStream.open("testlog1.log", std::fstream::in | std::fstream::out | std::fstream::app);
  b = new Server4();
  b->logStream.open("testlog2.log", std::fstream::in | std::fstream::out | std::fstream::app);


  Client* bert = new Client();
  bert->name = "bert";
  Client* ernie = new Client();
  ernie->name = "ernie";

  a->addClient(bert);
  b->addClient(ernie);


}

void ServerServerTest::tearDown(void) {
  // finally delete objects
  delete a;
  delete b;
}

void ServerServerTest::transmissionTest(void) {

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
  threadB->start(&serverAinB, b);


  Socket* socketA = listenSocket->acceptConn();
  threadA->start(socketA, a);

  usleep(2 * 1000000);

  Client* kermit = new Client();
  kermit->name = "kermit";
  Message* kermitMsg = new Message();
  kermitMsg->type = CLIENT_ADDED;
  kermitMsg->addParameter("kermit");
  kermitMsg->buildRawData();
  Client* cookiemonster = new Client();
  cookiemonster->name = "cookiemonster";
  Message cookiemonsterMsg;
  cookiemonsterMsg.type = CLIENT_ADDED;
  cookiemonsterMsg.addParameter("cookiemonster");
  cookiemonsterMsg.buildRawData();
  a->addClient(kermit);
  a->addBroadcast(kermitMsg);
  b->addClient(cookiemonster);
  b->addBroadcast(&cookiemonsterMsg);

  usleep(2 * 1000000);


  int matches = 0;
  for (int i = 0; i < a->clients.size(); i++) {
    for (int j = 0; j < b->clients.size(); j++) {
      if (a->clients[i]->name == b->clients[j]->name)
        matches++;
    }
  }

  threadB->stop(false);
  threadA->stop(false);




  CPPUNIT_ASSERT_EQUAL((int) a->clients.size(), (int) b->clients.size());
  CPPUNIT_ASSERT_EQUAL(matches, (int) a->clients.size());
  CPPUNIT_ASSERT_EQUAL(4, (int) a->clients.size());
}

