CC = gcc
CFLAGS= -Wall -O3

all: client.out server.out

client.out:	client/client.c
	$(CC) $(CFLAGS) client/client.c -o client.out -lcurses

server.out: server/server.c server/server.h server/main.c
	$(CC) $(CFLAGS) server/server.c server/main.c -o server.out

clean:
	rm -rf server.out
	rm -rf client.out
