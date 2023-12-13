#ifndef SOCKET_CLIENT_H
#define SOCKET_CLIENT_H

#include <sys/types.h>

ssize_t send_server_message(int client_socket, const char *message);
ssize_t receive_server_message(int client_socket, char *buffer, size_t buffer_size);
void close_connection(int sockfd);
int hostname_to_ip(const char *hostname, char *ip);
int connect_to_server(const char *server_name, int port_number);

#endif