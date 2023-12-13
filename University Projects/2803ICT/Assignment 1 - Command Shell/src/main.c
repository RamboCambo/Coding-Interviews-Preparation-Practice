#include <stdio.h>
#include <string.h>
#include "time_utils.h"
#include "path.h"
#include "sys.h"
#include "command_utils.h"
#include "put_utils.h"
#include "calc_utils.h"
#include "constants.h"
#include "file_utils.h"
#include <sys/stat.h>

int main() {
    char input[INPUT_MAX_LENGTH]; // buffer to hold user input
    char command[INPUT_COMMAND_MAX_LENGTH]; // buffer to hold command name
    char args[INPUT_ARGS_MAX_LENGTH]; // buffer to hold command arguments

    while (1) { // loop indefinitely
        printf("Enter a command: "); // prompt user for input
        fgets(input, sizeof(input), stdin); // read input from user
        sscanf(input, "%s %[^\n]", command, args); // parse command and arguments from input

        if (strcmp(command, "time") == 0) { // if command is "time"
            print_time(); // print current time
        } else if (strcmp(command, "path") == 0) { // if command is "path"
            print_path(); // print current working directory
        } else if (strcmp(command, "sys") == 0) { // if command is "sys"
            print_sys_info(); // print system information
        } else if(strcmp(command, "get") == 0) { // if command is "get"
            print_file_contents(args); // print contents of specified file
        } else if (strcmp(command, "put") == 0) { // if command is "put"
            create_dir_and_copy_files(args); // create directory and copy files
        } else if (strcmp(command, "quit") == 0) { // if command is "quit"
            printf("Exiting...\n"); // print exit message
            break; // exit loop
        } else if (strcmp(command, "calc") == 0) { // if command is "calc"
            printf("Result: %d\n", calculate(args)); // calculate and print result
        } else { // if command is not recognised
            printf("Command not recognised.\n"); // print error message
        }

        clear_input(command, args); // clear input buffers
    }

    return 0; // exit program
}