#ifndef SOCKET_SERVER_H
#define SOCKET_SERVER_H

#include <sys/types.h>

#define MAX_CLIENTS 10

void wait_for_clients(int server_fd, int *client_sockets, int player_count);
ssize_t send_client_message(int client_socket, const char *message);
int create_server_socket(int port, int player_count);
int accept_client(int server_fd);
void close_client_socket(int socket_fd);
ssize_t receive_client_message(int client_socket, char *buffer, size_t buffer_size);
void close_server_socket(int server_fd);

#endif