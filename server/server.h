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

typedef struct client_t Client;
typedef struct client_node_t ClientNode;
typedef struct server_t Server;

struct client_t{

    struct sockaddr_in client_socket;
    int client_socket_num;
    Server *server_data;
    
};

struct client_node_t{

    Client *client;
    ClientNode *next_client;

};


struct server_t{

    ClientNode *node;
    int clients_count;
};


Server *init_Server(void);
Client *add_client(Server **server_data, struct sockaddr_in client_socket, int client_socket_num);
void remove_client(Server **server_data, Client *client);
void *connexion_handler(void *clients);
#endif