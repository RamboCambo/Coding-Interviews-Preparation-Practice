#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>

void close_connection(int sockfd)
{
    close(sockfd);
}

ssize_t send_server_message(int sockfd, const char *message)
{
    size_t message_len = strlen(message);
    ssize_t bytes_sent = send(sockfd, message, message_len, 0);

    if (bytes_sent == -1)
    {
        perror("send");
        close_connection(sockfd);
        exit(EXIT_FAILURE);
    }

    // printf("Sent message: %s\n", message);

    return bytes_sent;
}

ssize_t receive_server_message(int sockfd, char *buffer, size_t buffer_size)
{
    ssize_t bytes_received = read(sockfd, buffer, buffer_size);
    // printf("Bytes received: %ld\n", bytes_received);

    if (bytes_received == -1)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';

    return bytes_received;
}

// https://www.binarytides.com/hostname-to-ip-address-c-sockets-linux/
int hostname_to_ip(const char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        // get the host info
        perror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **)he->h_addr_list;

    for (i = 0; addr_list[i] != NULL; i++)
    {
        // Return the first one;
        strcpy(ip, inet_ntoa(*addr_list[i]));
        return 0;
    }

    return 1;
}

int connect_to_server(const char *server_name, int port_number)
{
    int sockfd;
    struct sockaddr_in serv_addr;
    char ip[INET_ADDRSTRLEN];

    // Resolve the IP address of the server
    if (hostname_to_ip(server_name, ip) != 0)
    {
        fprintf(stderr, "Failed to resolve IP address of server\n");
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Set up the server address
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_number);
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
    {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}