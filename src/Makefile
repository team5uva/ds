CPPUNIT_PATH=/opt/cppunit
Main : Server4.cpp Admin.h Client.h Logger.h Message.h Server.h Server4.h Socket.h Thread.h Thread.cpp Socket.cpp configFile.cpp configFile.h Message.cpp
	g++ -pthread Server4.cpp Thread.cpp Socket.cpp configFile.cpp Message.cpp -o Main
	
Test: Testing.o MessageTest.o Message.o
	gcc -o Testing Testing.o MessageTest.o Socket.cpp Message.o -L${CPPUNIT_PATH}/lib -lstdc++ -lcppunit -ldl

Message.o: Message.cpp Message.h 
	gcc -c Message.cpp

MessageTest.o: MessageTest.cpp
	gcc -c MessageTest.cpp -I${CPPUNIT_PATH}/include

Testing.o: Testing.cpp
	gcc -c Testing.cpp -I${CPPUNIT_PATH}/include
	
clean: 
	rm -rf *.o Main Testing
