#include <stdio.h>
#include "constants.h"

// print the contents of a file
void print_file_contents(char *file_name) {
    FILE *fp; // file pointer
    char buffer[MAX_LINE_LENGTH]; // buffer to hold file contents
    int line_count = 1; // line counter

    // open the file for reading
    fp = fopen(file_name, "r");
    if (fp == NULL) { // if the file could not be opened
        printf("Error opening \"%s\"!\n", file_name); // print error message
        return; // exit function
    }

    // print header message
    printf("Printing file \"%s\" contents...\n", file_name);

    // read the file line by line
    while(fgets(buffer, MAX_LINE_LENGTH, fp) != NULL) {
        printf("%d: %s", line_count, buffer); // print line number and contents
        line_count++; // increment line counter

        // pause every 40 lines to allow user to read output
        if(line_count % 40 == 1) {
            printf("Press any ENTER to continue...");
            getchar();
        }
    }

    // close the file
    fclose(fp);
}