#ifndef SERVERSERVERTEST_H
#define SERVERSERVERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Server4.h"

using namespace std;

class ServerServerTest : public CPPUNIT_NS :: TestFixture
{

    CPPUNIT_TEST_SUITE (ServerServerTest);
    CPPUNIT_TEST (transmissionTest);
    CPPUNIT_TEST_SUITE_END ();


    public:
        void setUp (void);
        void tearDown (void);

    protected:
        void transmissionTest (void);
    private:
        Server4 *a, *b;
};

#endif
