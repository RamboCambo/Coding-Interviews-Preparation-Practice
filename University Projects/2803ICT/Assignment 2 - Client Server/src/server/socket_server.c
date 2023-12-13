#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "socket_server.h"
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

ssize_t send_client_message(int client_socket, const char *message)
{
    return send(client_socket, message, strlen(message), 0);
}

ssize_t receive_client_message(int client_socket, char *buffer, size_t buffer_size)
{
    ssize_t bytes_received = read(client_socket, buffer, buffer_size);
    // printf("Bytes received: %ld\n", bytes_received);
    buffer[bytes_received] = '\0';
    return bytes_received;
}

void wait_for_clients(int server_fd, int *client_sockets, int player_count)
{
    int num_connected_clients = 0;

    // Wait for clients to connect
    while (num_connected_clients < player_count)
    {
        int new_socket = accept_client(server_fd);

        // Add client socket to array
        client_sockets[num_connected_clients] = new_socket;
        num_connected_clients++;

        printf("Client connected\n");
    }
}

int create_server_socket(int port, int player_count)
{
    int server_fd, opt = 1;
    struct sockaddr_in address;

    // Create socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("ERROR socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        printf("ERROR setsockopt");
        close_server_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        printf("ERROR bind failed");
        close_server_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, player_count) == -1)
    {
        printf("ERROR listen");
        close_server_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    return server_fd;
}

int accept_client(int server_fd)
{
    int new_socket, addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in address;

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) == -1)
    {
        printf("ERROR accept");
        close_client_socket(server_fd);
        exit(EXIT_FAILURE);
    }

    // Make client socket non-blocking
    if (fcntl(new_socket, F_SETFL, O_NONBLOCK) == -1)
    {
        printf("ERROR fcntl");
        close_client_socket(new_socket);
        exit(EXIT_FAILURE);
    }

    return new_socket;
}

void close_client_socket(int socket_fd)
{
    if (close(socket_fd) == -1)
    {
        printf("ERROR close");
        exit(EXIT_FAILURE);
    }

    // closing the listening socket
    shutdown(socket_fd, SHUT_RDWR);
}

void close_server_socket(int server_fd)
{
    if (close(server_fd) == -1)
    {
        printf("ERROR close");
        exit(EXIT_FAILURE);
    }

    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
}