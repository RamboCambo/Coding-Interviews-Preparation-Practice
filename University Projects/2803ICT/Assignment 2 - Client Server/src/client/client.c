#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "socket_client.h"
#include "client.h"
#include <stdbool.h>

int main(int argc, char const *argv[])
{
    // Check if the client is called with the correct number of arguments
    if (argc != 4)
    {
        printf(CLIENT_USAGE_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    // Get the port number, game type, and game arguments from the command line
    const char *game_type = argv[1];
    const char *server_name = argv[2];
    const int port_number = atoi(argv[3]);

    printf("Game type: %s\n", game_type);
    printf("Server name: %s\n", server_name);
    printf("Port number: %d\n", port_number);

    int server_fd = connect_to_server(server_name, port_number);

    char response[BUFFER_SIZE];
    char line1[BUFFER_SIZE];
    char line2[BUFFER_SIZE];

    ssize_t response_bytes = receive_server_message(server_fd, response, BUFFER_SIZE);

    while (1)
    {
        if (response_bytes > 0)
        {
            // Find the first newline character
            char *newline1 = strchr(response, '\n');
            if (newline1 == NULL)
            {
                printf("Error: buffer does not contain two lines. Quitting game!\n");
                send_server_message(server_fd, "QUIT");
                close_connection(server_fd);
                exit(EXIT_FAILURE);
            }

            // Copy the first line into line1
            size_t line1_len = newline1 - response;
            memcpy(line1, response, line1_len);
            line1[line1_len] = '\0';

            // Find the second newline character
            char *newline2 = strchr(newline1 + 1, '\n');
            if (newline2 == NULL) {
                printf("Error: buffer does not contain two lines. Quitting game!\n");
                send_server_message(server_fd, "QUIT");
                close_connection(server_fd);
                exit(EXIT_FAILURE);
            }

            // Copy the second line into line2
            size_t line2_len = newline2 - newline1 - 1;
            memcpy(line2, newline1 + 1, line2_len);
            line2[line2_len] = '\0';

            // Check the content of line 1
            char *token = strtok(line1, " ");
            if (strcmp(token, "TEXT") == 0)
            {
                // Handle TEXT logic
                token = strtok(NULL, ""); // Get everything after the first token
                printf("%s", token);
            }
            else
            {
                printf("Incorrect protocol received from server. Quitting game!\n");
                send_server_message(server_fd, "QUIT");
                close_connection(server_fd);
                exit(EXIT_FAILURE);
            }

            // Check the content of line 2
            token = strtok(line2, "");
            if (strcmp(token, "GO") == 0)
            {
                // Ask for user input
                // Only supposed to give a response between 1-9 so tiny buffer
                char input[20];
                fgets(input, 20, stdin);

                // Strip invisible characters from the user input
                size_t input_len = strlen(input);
                if (input_len > 0 && (input[input_len - 1] == '\n' || input[input_len - 1] == '\r'))
                {
                    input[input_len - 1] = '\0';
                }

                // Check if the user input is "quit" (case-insensitive)
                if (strcasecmp(input, "quit") == 0)
                {
                    // Handle QUIT logic
                    printf("Quitting...\n");
                    send_server_message(server_fd, "QUIT");
                    close_connection(server_fd);
                    exit(EXIT_SUCCESS);
                }

                // Add "MOVE " to the front of the user input
                char message[BUFFER_SIZE];
                snprintf(message, BUFFER_SIZE, "MOVE %s", input);

                // Send user input to server
                send_server_message(server_fd, message);
            }
            else if (strcmp(token, "END") == 0)
            {
                printf("\n");
                close_connection(server_fd);
                exit(EXIT_SUCCESS);
            }
            else
            {
                printf("Incorrect protocol received from server. Quitting game!\n");
                send_server_message(server_fd, "QUIT");
                close_connection(server_fd);
                exit(EXIT_FAILURE);
            }
        }
        else if (response_bytes == -1)
        {
            printf("Error receiving message from server\n");
            close_connection(server_fd);
            exit(EXIT_FAILURE);
        }
        response_bytes = receive_server_message(server_fd, response, BUFFER_SIZE);
    }
    
    close_connection(server_fd);
    return 0;
}
