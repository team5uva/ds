Main : ServerThread.cpp ClientThread.cpp Server4.cpp Server.cpp Admin.h Client.h Logger.h Message.h Server.h Server4.h Socket.h Thread.h Thread.cpp Socket.cpp configFile.cpp configFile.h Message.cpp main.cpp
	g++ -pthread -g Server4.cpp Server.cpp ServerThread.cpp ClientThread.cpp Thread.cpp Socket.cpp configFile.cpp Message.cpp main.cpp -o Main

test: ServerThread.cpp ClientThread.cpp Server4.cpp Server.cpp Admin.h Client.h Logger.h Message.h Server.h Server4.h Socket.h Thread.h Thread.cpp Socket.cpp configFile.cpp configFile.h Message.cpp Testing.cpp MessageTest.cpp MessageTest.h
	g++ -lcppunit -pthread -g Server4.cpp Server.cpp ServerThread.cpp ClientThread.cpp Thread.cpp Socket.cpp configFile.cpp Message.cpp Testing.cpp MessageTest.cpp -o Test
