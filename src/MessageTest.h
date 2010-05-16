#ifndef MESSAGETEST_H
#define MESSAGETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include "Message.h"

using namespace std;

class MessageTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE(MessageTest);
  CPPUNIT_TEST(LocalParseMessageTest);
  CPPUNIT_TEST(SocketParseMessageTest);
  CPPUNIT_TEST_SUITE_END();


public:
  void setUp(void);
  void tearDown(void);

protected:
  void LocalParseMessageTest(void);
  void SocketParseMessageTest(void);
private:
  Message *a, *b;
};

#endif
