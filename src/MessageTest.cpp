#include "MessageTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION (MessageTest);

void MessageTest :: setUp (void)
{
    // set up test environment (initializing objects)
    a = new Message();
    a->type = 10;
    a->addParameter("test");
    a->buildRawData();
    b = new Message(a->rawData, a->length);
    b->parseData();
}

void MessageTest :: tearDown (void)
{
    // finally delete objects
    delete a; delete b;
}

void MessageTest :: ParseMessageTest (void)
{
    // check subtraction results
    CPPUNIT_ASSERT_EQUAL (a->type, b->type);
    CPPUNIT_ASSERT_EQUAL (a->length, b->length);
    CPPUNIT_ASSERT_EQUAL (a->rawData, b->rawData);    
    for(int i = 0; i < a->words.size(); i++) {
      CPPUNIT_ASSERT_EQUAL (a->words[i], b->words[i]);   
    }
}

