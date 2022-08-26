#include "server.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void broadcast_data(Client *client, char *buffer, int bytes_recv)
{
    ClientNode *client_ptr = client->server_data->node;
    int bytes_send;

    while (client_ptr != NULL){
        if (client_ptr->client->client_socket_num != client->client_socket_num){
            if ((bytes_send = send(client_ptr->client->client_socket_num, buffer, bytes_recv, 0)) == -1){
                printf("[-] Pipe broken for : %s on port -> %d !\n", inet_ntoa(client_ptr->client->client_socket.sin_addr), ntohs(client_ptr->client->client_socket.sin_port));
            }
            else if (bytes_send != bytes_recv){
                printf("[X] Broadcast corrupted for : %s on port -> %d !\n", inet_ntoa(client_ptr->client->client_socket.sin_addr), ntohs(client_ptr->client->client_socket.sin_port));
            }
    }
    client_ptr = client_ptr->next_client;
   }
}

Server *init_Server(void)
{
    Server *serv = (Server*)malloc(sizeof(Server));
    if (serv == NULL)
        return NULL;
    
    serv->clients_count = 0;
    serv->node = NULL;

    pthread_mutex_init(&mutex, NULL);
    return serv;
}


Client *add_client(Server **server_data, struct sockaddr_in client_socket, int client_socket_num)
{
    ClientNode *tmp;
    Client *curr_client;
    ClientNode *next_node;


    curr_client = (Client*)malloc(sizeof(Client));
    if (curr_client == NULL)
        return NULL;

    else{
        curr_client->client_socket = client_socket;
        curr_client->client_socket_num = client_socket_num;
        curr_client->server_data = *server_data;
    }
    next_node = (ClientNode*)malloc(sizeof(ClientNode));
    if (next_node == NULL)
        return NULL;
    else{
        next_node->client = curr_client;
        next_node->next_client = NULL;
    }
    pthread_mutex_lock(&mutex);
    tmp = (*server_data)->node;
    if (tmp == NULL)
        (*server_data)->node = next_node;

    else{
        while (tmp->next_client != NULL)
            tmp = tmp->next_client;

        tmp->next_client = next_node;
    }

    (*server_data)->clients_count++;
    pthread_mutex_unlock(&mutex);
    return curr_client;
}

void remove_client(Server **server_data, Client *client)
{
    ClientNode *node_ptr = NULL;
    ClientNode *node_prec = NULL;

    pthread_mutex_lock(&mutex);
    node_ptr = (*server_data)->node;

    if (node_ptr != NULL){
        while (node_ptr->next_client != NULL && node_ptr->client->client_socket_num != client->client_socket_num){
            node_prec = node_ptr;
            node_ptr = node_ptr->next_client;
        }
        
        if (node_prec != NULL){
            close(node_ptr->client->client_socket_num);
            node_prec->next_client = node_ptr->next_client;
        }
        else
            (*server_data)->node = (*server_data)->node->next_client;
    }
    (*server_data)->clients_count--;
    pthread_mutex_unlock(&mutex);
}

void *connexion_handler(void *client_data)
{
    Client *client = (Client*)client_data;
    char buffer[BUFFERSIZE];
    int bytes_received;

    while((bytes_received = recv(client->client_socket_num, buffer, BUFFERSIZE -1, 0)) > 0){

        if (buffer[0] != '\n'){
            //printf("[!] Received %d bytes from : %s on port -> %d !\n", bytes_received, inet_ntoa(client->client_socket.sin_addr), ntohs(client->client_socket.sin_port));
            
            broadcast_data(client, buffer, bytes_received);

        }
    }
    if (bytes_received == 0){
        printf("[-] Client from : %s on port -> %d disconnected !\n", inet_ntoa(client->client_socket.sin_addr), ntohs(client->client_socket.sin_port));
    }
    else if (bytes_received == -1){
        printf("[-] Pipe broken for client from : %s on port -> %d !\n", inet_ntoa(client->client_socket.sin_addr), ntohs(client->client_socket.sin_port));
    }
    remove_client(&client->server_data, client);

   return 0;

}
