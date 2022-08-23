#include "server.h"

void *connexion_handler (void *arguments)
{
    ClientArgs *client_args = (ClientArgs*)arguments;
    int bytes_rcv, bytes_send, i;
    char *buffer = NULL;

    bytes_rcv = bytes_send = 0;

    do{
        buffer = (char*)malloc(sizeof(char)*BUFFERSIZE);
        if (buffer == NULL){
            printf("Fatal malloc error on buffer !\n");
            bytes_rcv = -1;
        }
        else{
            bytes_rcv = recv(client_args->client_socket, buffer, BUFFERSIZE - 1, 0);
            if (bytes_rcv == 0){
                printf("Client from : %s on port -> %d disconnected !\n", inet_ntoa(client_args->client.sin_addr), ntohs(client_args->client.sin_port));
                socket_disconnect_handler(client_args);
                client_args->server_data->sockets_count--;
                bytes_send = 0;
            }
            else if (bytes_rcv == -1){
                printf("Pipe broken for : %s on port -> %d disconnected !\n", inet_ntoa(client_args->client.sin_addr), ntohs(client_args->client.sin_port));
                socket_disconnect_handler(client_args);
                client_args->server_data->sockets_count--;
                bytes_send = 0;
            }
            else{
                buffer[bytes_rcv] = '\0';
                if (buffer[0] != '\n'){
                    for (i = 0; i < client_args->server_data->sockets_count; i++){
                        if (client_args->client_socket != client_args->server_data->sockets_arr[i]){
                            bytes_send = send(client_args->server_data->sockets_arr[i], buffer, strlen(buffer) + 1, 0);
                            if (bytes_send < 1){
                                printf("Could not send message back !\n");
                                socket_disconnect_handler(client_args);
                            }
                            else if (bytes_send != strlen(buffer)+1){
                                printf("Data corrupted !\n");
                                socket_disconnect_handler(client_args);
                            }
                        }
                    }
                }
            }
            free(buffer);

        }
    }while (bytes_rcv > 0 || bytes_send > 0);

    close(client_args->client_socket);
    free(client_args);

    return 0;
}

Server *InitServ(int max_client)
{
    Server *server_data = (Server*)malloc(sizeof(Server));
    if (server_data == NULL){
        printf("Fatal Malloc error !\n");
        return NULL;
    }
    server_data->sockets_count = 0;
    server_data->sockets_arr = (int*)malloc(sizeof(int)*max_client);
    if (server_data->sockets_arr == NULL){
        printf("Fatal malloc error for sockets_arrays !\n");
        return NULL;
    }

    return server_data;
}

void socket_disconnect_handler(ClientArgs *clientargs)
{
    assert(clientargs != NULL);
    int i;

    /*
        Find not valid socket in list
    */

    for (i = 0; i < clientargs->server_data->sockets_count && clientargs->server_data->sockets_arr[i] != clientargs->client_socket ; i++)
        ;
    
    for (; i < clientargs->server_data->sockets_count; i++){
        if ((i + 1) < clientargs->server_data->sockets_count)
            clientargs->server_data->sockets_arr[i] = clientargs->server_data->sockets_arr[i + 1];
    }


}