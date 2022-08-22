CC = gcc
CFLAGS= -Wall -O3

all: client.out server.out

client.out:	client/client.c
	$(CC) $(CFLAGS) client/client.c -o client.out

server.out: server/server.c
	$(CC) $(CFLAGS) server/server.c -o server.out

clean:
	rm -rf server.out
	rm -rf client.out
