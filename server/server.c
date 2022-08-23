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
            bytes_rcv = recv(client_args->client_socket, buffer, BUFFERSIZE, 0);
            if (bytes_rcv == 0){
                printf("Client disconnected !\n");
                socket_disconnect_handler(client_args->client_socket, client_args->server_data);
                client_args->server_data->sockets_count--;
                bytes_send = 0;
            }
            else if (bytes_rcv == -1){
                printf("Pipe broken for client !\n");
                socket_disconnect_handler(client_args->client_socket, client_args->server_data);
                client_args->server_data->sockets_count--;
                bytes_send = 0;
            }
            else{
                buffer[bytes_rcv] = '\0';
                if (buffer[0] != '\n'){
                    for (i = 0; i < client_args->server_data->sockets_count; i++){
                        if (client_args->client_socket != client_args->server_data->sockets_arr[i]){
                            bytes_send = send(client_args->server_data->sockets_arr[i], buffer, strlen(buffer) + 1, 0);
                            if (bytes_send < 1)
                                printf("Could not send message back !\n");
                            else if (bytes_send != strlen(buffer)+1)
                                printf("Data corrupted !\n");
                        }
                    }
                }
            }
            free(buffer);

        }
    }while (bytes_rcv > 0 || bytes_send > 0);

    printf("closing !\n");
    close(client_args->client_socket);
    free(client_args);

    return 0;
}

Server *InitServ(void)
{
    Server *server_data = (Server*)malloc(sizeof(Server));
    if (server_data == NULL){
        printf("Fatal Malloc error !\n");
        return NULL;
    }
    server_data->sockets_count = 0;
    server_data->sockets_arr = (int*)malloc(sizeof(int));
    if (server_data->sockets_arr == NULL){
        printf("Fatal malloc error for sockets_arrays !\n");
        return NULL;
    }

    return server_data;
}

void socket_disconnect_handler(int socket_num, Server *server_data)
{
    assert(socket_num >= 0);
    assert(server_data->sockets_arr != NULL);
    int i;

    /*
        Find not valid socket in list
    */

    for (i = 0; i < server_data->sockets_count && server_data->sockets_arr[i] != socket_num ; i++)
        ;
    
    for (; i < server_data->sockets_count; i++){
        if ((i + 1) < server_data->sockets_count)
            server_data->sockets_arr[i] = server_data->sockets_arr[i + 1];
            
    }

}