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
#include <signal.h>
#include <fcntl.h>

#include "server.h"

extern pthread_mutex_t mutex;
int main(int argc, char **argv)
{
    signal(SIGPIPE, SIG_IGN);

    int socket_desc, client_socket, c;
    struct sockaddr_in server, client;
    pthread_t sniffer_thread;

    Client *client_data = NULL;
    Server *server_data = NULL;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1){
        printf("Could not create socket !\n");
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9999);


    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
        printf("Error setsockopt !\n");
        return -1;
    }

    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0){
        printf("Bind failed !\n");
        return -1;
    }

    server_data = init_Server();
    if (server_data == NULL){
        printf("Malloc error on server struct !\n");
        return -1;
    }

    printf("Listening for any connection on %s\n", inet_ntoa(server.sin_addr));
    listen(socket_desc, 128);

    while(1){
        c = sizeof(struct sockaddr_in);
        do{
            client_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)&c);
            
        }while(client_socket < 0);

        if (client_socket < 0)
            printf("Accept Failed !\n");
        else{
            printf("[+] New connection from : %s on port -> %d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            client_data = add_client(&server_data, client, client_socket);
            if (pthread_create(&sniffer_thread, NULL, connexion_handler,(void *)client_data) < 0){
                printf("Could not create thread !\n");
                return -1;
            }
        }
    }


}