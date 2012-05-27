CC = cc
CFLAGS = -Wall

HEADER = headerT.c headerT.h

all: server client

server: server.c $(HEADER)
	$(CC) $(CFLAGS) -c server.c
	$(CC) $(CFLAGS) -c headerT.c
	$(CC) server.o headerT.o -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client
