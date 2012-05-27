CC = cc
CFLAGS = -Wall

HEADER = headerT.c headerT.h

all: server test

server: server.c $(HEADER)
	$(CC) $(CFLAGS) server.c -o server

test: test.c $(HEADER)
	$(CC) $(CFLAGS) test.c -o test
