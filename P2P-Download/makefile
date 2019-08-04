all:  Server Client
Client:Client.cpp
	g++ -std=c++0x $^ -o $@ -lpthread -lboost_thread -lboost_filesystem -lboost_system
Server:Server.cpp
	g++ -std=c++0x $^ -o $@ -lpthread -lboost_filesystem -lboost_system
