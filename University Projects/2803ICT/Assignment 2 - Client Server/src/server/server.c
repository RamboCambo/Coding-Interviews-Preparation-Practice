#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "server.h"
#include "socket_server.h"
#include "time_utils.h"
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    // Check if the server is called with the correct number of arguments
    if (argc != 4)
    {
        printf(SERVER_USAGE_ERROR_MESSAGE);
        exit(EXIT_FAILURE);
    }

    // Get the port number, game type, and game arguments from the command line
    const int port_number = atoi(argv[1]);
    const char *game_type = argv[2];
    const int player_count = atoi(argv[3]);

    printf("Port number: %d\n", port_number);
    printf("Game type: %s\n", game_type);
    printf("Player count: %d\n", player_count);

    // Create server socket
    int server_fd = create_server_socket(port_number, player_count);
    int client_sockets[MAX_CLIENTS] = {0};
    int players_left = player_count;
    int sum = 0;

    // Wait for clients to connect
    wait_for_clients(server_fd, client_sockets, player_count);
    printf("All clients connected\n");

    while (players_left > 0)
    {
        // implement game error limit and if client perform 5 in a row disconnect them
        printf("Waiting for client input\n");
        for (int i = 0; i < player_count; i++)
        {
            // skip empty sockets
            if (client_sockets[i] == 0)
            {
                continue;
            }

            if(players_left == 1)
            {
                send_client_message(client_sockets[i], "TEXT You won!\nEND\n");
                close_client_socket(client_sockets[i]);
                players_left = 0;
                break;
            }

            char message[BUFFER_SIZE];
            sprintf(message, "TEXT Sum is %d. Enter number: \nGO\n", sum);

            send_client_message(client_sockets[i], message);

            char response[BUFFER_SIZE];
            uint64_t time_message_sent_ms = get_current_time_ms(),
                     current_time_ms = get_current_time_ms();

            int client_game_error_count = 0;

            // Don't know why but the clients don't block waiting for I/O from the server
            // But the server blocks on waiting for I/O from the client
            // Or maybe I haven't looked at it properly yet and the client does actually block
            // Maybe because it is a server and it setup like it is maybe it blocks
            // Who cares?! It works!
            ssize_t response_bytes = receive_client_message(client_sockets[i], response, BUFFER_SIZE);

            while (current_time_ms - time_message_sent_ms <= MAX_CLIENT_RESPONSE_TIME_MS && client_game_error_count < MAX_ERRORS)
            {
                if (response_bytes > 0)
                {
                    char *token = strtok(response, " ");
                    if (token != NULL)
                    {
                        if (strcmp(token, "MOVE") == 0)
                        {
                            // Handle MOVE logic
                            printf("MOVE command received\n");

                            // Get the second token
                            token = strtok(NULL, " ");
                            if (token != NULL)
                            {
                                // Check if the second token is a number
                                char *endptr;
                                long int value = strtol(token, &endptr, 10);
                                if (*endptr == '\0')
                                {
                                    // Handle number case
                                    printf("Number value: %ld\n", value);
                                    if (value >= 1 && value <= 9)
                                    {
                                        sum += value;
                                        printf("Sum is now %d\n", sum);
                                        
                                        if (sum >= TARGET_SUM)
                                        {
                                            send_client_message(client_sockets[i], "TEXT You won!\nEND\n");
                                            close_client_socket(client_sockets[i]);
                                            for (int j = 0; j < player_count; j++)
                                            {
                                                if (i != j)
                                                {
                                                    send_client_message(client_sockets[j], "TEXT You lost!\nEND\n");
                                                    close_client_socket(client_sockets[j]);
                                                }
                                            }
                                            players_left = 0;
                                        }

                                        break;
                                    }
                                    else
                                    {
                                        printf("Number value outside 1-9 range: %ld\n", value);
                                        client_game_error_count++;
                                        send_client_message(client_sockets[i], "TEXT ERROR Bad input. Try again: \nGO\n");
                                        time_message_sent_ms = get_current_time_ms();
                                    }
                                }
                                else
                                {
                                    // Handle non-number case
                                    printf("Non-number value: %s\n", token);
                                    client_game_error_count++;
                                    send_client_message(client_sockets[i], "TEXT ERROR Bad input. Try again: \nGO\n");
                                    time_message_sent_ms = get_current_time_ms();
                                }
                            }
                            else
                            {
                                // Handle no second token case
                                printf("No second token\n");
                                client_game_error_count++;
                                send_client_message(client_sockets[i], "TEXT ERROR Bad input. Try again: \nGO\n");
                                time_message_sent_ms = get_current_time_ms();
                            }
                        }
                        else if (strcmp(token, "QUIT") == 0)
                        {
                            // Handle QUIT logic
                            // disconnect client
                            close_client_socket(client_sockets[i]);
                            client_sockets[i] = 0;
                            players_left--;
                            printf("QUIT command received\n");
                            break;
                        }
                        else
                        {
                            // Handle other cases
                            printf("Unknown command received\n");
                            send_client_message(client_sockets[i], "TEXT ERROR Bad input. Try again: \nGO\n");
                            client_game_error_count++;
                            time_message_sent_ms = get_current_time_ms();
                        }
                    }
                }
                // else if (response_bytes == -1)
                // {
                //     // disconnect client
                //     send_client_message(client_sockets[i], "TEXT ERROR Error receiving response. Disconnecting...\nEND\n");
                //     close_client_socket(client_sockets[i]);
                //     client_sockets[i] = 0;
                //     players_left--;
                //     break;
                // }
                response_bytes = receive_client_message(client_sockets[i], response, BUFFER_SIZE);
                current_time_ms = get_current_time_ms();
            }

            if(client_game_error_count == MAX_ERRORS)
            {
                send_client_message(client_sockets[i], "TEXT ERROR Too many game errors. Disconnecting...\nEND\n");
                close_client_socket(client_sockets[i]);
                client_sockets[i] = 0;
                players_left--;
                printf("Client disconnected due to too many game errors\n");
            } 
            else if (current_time_ms - time_message_sent_ms >= MAX_CLIENT_RESPONSE_TIME_MS)
            {
                // disconnect client
                send_client_message(client_sockets[i], "TEXT ERROR Timed out. Disconnecting...\nEND\n");
                close_client_socket(client_sockets[i]);
                client_sockets[i] = 0;
                players_left--;
                printf("Client timed out\n");
            }
        }
    }

    // Close server socket
    close_server_socket(server_fd);

    return 0;
}
