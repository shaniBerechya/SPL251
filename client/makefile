CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread
# TODO change the makefile for StompEMIlient
all: StompClient

StompClient: bin/ConnectionHandler.o bin/StompFrame.o bin/StompProtocol.o bin/event.o bin/StompClient.o
	g++ -o bin/StompClient bin/ConnectionHandler.o bin/StompFrame.o bin/StompProtocol.o bin/event.o bin/StompClient.o $(LDFLAGS)

StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o
	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp

bin/StompProtocol.o: src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

.PHONY: clean
clean:
	rm -f bin/*
	