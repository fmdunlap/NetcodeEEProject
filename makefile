OBJS = client.o edge.o server_or.o server_and.o
EXES = client edge serverand serveror
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)

all : client build_edge build_serverand build_serveror

build_edge : edge.o
	$(CC) $(LFLAGS) edge.o -o edge

edge.o : edge.cpp edge.h
	$(CC) $(CFLAGS) $(DEBUG) edge.cpp

client : client.o
	$(CC) $(LFLAGS) client.o -o client

client.o : client.cpp client.h
	$(CC) $(CFLAGS) $(DEBUG) client.cpp

build_serverand : server_and.o
	$(CC) $(LFLAGS) server_and.o -o serverand

server_and.o : server_and.cpp server_and.h
	$(CC) $(CFLAGS) $(DEBUG) server_and.cpp

build_serveror : server_or.o
	$(CC) $(LFLAGS) server_or.o -o serveror

server_or.o : server_or.cpp server_or.h
	$(CC) $(CFLAGS) $(DEBUG) server_or.cpp

server_or : build_serveror
	./serveror

server_and : build_serverand
	./serverand

edge : build_edge
	./edge

.PHONY : clean
clean :
	rm $(OBJS)
	rm $(EXES)
