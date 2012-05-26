CC = cc
CFLAGS = -Wall

server: server.c
	$(CC) $(CFLAGS) server.c -o server
