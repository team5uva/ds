Main : Server4.o Server.o Thread.o Socket.o configFile.o configFile.h Message.o main.o ServerThread.o ClientThread.o
	g++ -pthread -g Server4.o Server.o Thread.o ClientThread.o ServerThread.o Socket.o configFile.o Message.o main.o -o Main

test: Server4.o Server.o Thread.o Socket.o configFile.o configFile.h Message.o ServerThread.o ClientThread.o Testing.o MessageTest.o ServerServerTest.o 
	g++ -lcppunit -pthread -g  Server4.o Server.o Thread.o ClientThread.o ServerThread.o Socket.o configFile.o Message.o Testing.o MessageTest.o ServerServerTest.o -o Test

Thread.o: Thread.h Server.h Client.h Message.h MessageType.h Thread.cpp 
	g++ -pthread -g -c Thread.cpp 

ServerThread.o: Thread.h Server.h Client.h Message.h MessageType.h ServerThread.cpp 
	g++ -pthread -g -c ServerThread.cpp 

ClientThread.o: Thread.h Server.h Client.h Message.h MessageType.h ClientThread.cpp
	g++ -pthread -g -c ClientThread.cpp 

Server.o : Server4.h Message.h Client.h Socket.h MessageType.h Server.h Server.cpp
	g++ -pthread -g -c Server.cpp

Server4.o : MessageType.h Admin.h Thread.h configFile.h Message.h Client.h Socket.h MessageType.h Server.h Server4.h Server4.cpp
	g++ -pthread -g -c Server4.cpp

Socket.o: Socket.h Socket.cpp
	g++ -pthread -g -c Socket.cpp

main.o : Thread.h Server4.h main.cpp
	g++ -pthread -g -c main.cpp

Message.o : Message.h Message.cpp Socket.h MessageType.h
	g++ -pthread -g -c Message.cpp

configFile.o : configFile.h configFile.cpp
	g++ -pthread -g -c configFile.cpp

Testing.o : Testing.cpp
	g++ -pthread -g -c Testing.cpp

MessageTest.o : MessageTest.cpp MessageTest.h
	g++ -pthread -g -c MessageTest.cpp

ServerServerTest.o : ServerServerTest.cpp ServerServerTest.h
	g++ -pthread -g -c ServerServerTest.cpp

clean:
	rm -f *.o Main Test
