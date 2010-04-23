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
/*

void MessageTest :: subTest (void)
{
    // check addition results
    CPPUNIT_ASSERT_EQUAL (*a - *b, Fraction (-1, 6));
    CPPUNIT_ASSERT_EQUAL (*b - *c, Fraction (1, 3));
    CPPUNIT_ASSERT_EQUAL (*b - *c, Fraction (2, 6));
    CPPUNIT_ASSERT_EQUAL (*d - *e, Fraction (0));
    CPPUNIT_ASSERT_EQUAL (*d - *e - *f - *g - *h, Fraction (-5));
}

void MessageTest :: exceptionTest (void)
{
    // an exception has to be thrown here
    CPPUNIT_ASSERT_THROW (Fraction (1, 0), DivisionByZeroException);
}

void MessageTest :: equalTest (void)
{
    // test successful, if true is returned
    CPPUNIT_ASSERT (*d == *e);
    CPPUNIT_ASSERT (Fraction (1) == Fraction (2, 2));
    CPPUNIT_ASSERT (Fraction (1) != Fraction (1, 2));
    // both must have equal valued
    CPPUNIT_ASSERT_EQUAL (*f, *g);
    CPPUNIT_ASSERT_EQUAL (*h, Fraction (0));
    CPPUNIT_ASSERT_EQUAL (*h, Fraction (0, 1));
}
*/
