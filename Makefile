CC = cc
CFLAGS = -Wall

HEADER = headerT.c headerT.h

all: server

server: server.c $(HEADER)
	$(CC) $(CFLAGS) -c server.c
	$(CC) $(CFLAGS) -c headerT.c
	$(CC) server.o headerT.o -o server

test: test.c $(HEADER)
	$(CC) $(CFLAGS) test.c -o test
