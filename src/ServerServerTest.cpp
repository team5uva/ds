#include "ServerServerTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION (ServerServerTest);

void ServerServerTest :: setUp (void)
{
    // set up test environment (initializing objects)
    a = new Server4();
    b = new Server4();
}

void ServerServerTest :: tearDown (void)
{
    // finally delete objects
    delete a; delete b;
}

void ServerServerTest :: transmissionTest (void)
{
}

