#include <stdio.h>
#include <unistd.h>

// print the current working directory
void print_path() {
    char cwd[1024]; // buffer to hold current working directory

    // get the current working directory
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working directory: %s\n", cwd); // print the current working directory
    } else {
        perror("getcwd() error"); // print an error message if the current working directory could not be obtained
    }
}