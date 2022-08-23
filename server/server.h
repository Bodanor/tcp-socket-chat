#ifndef __SERVER_H__
#define __SERVER_H__

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#define BUFFERSIZE 255

typedef struct server_data
{
    int sockets_count;
    int *sockets_arr;

}Server;

typedef struct arg_handler
{
    int client_socket;
    struct sockaddr_in client;
    Server *server_data;

}ClientArgs;

void *connexion_handler (void *arguments);
Server *InitServ(void);
void socket_disconnect_handler(int socket_num, Server *server_data);

#endif