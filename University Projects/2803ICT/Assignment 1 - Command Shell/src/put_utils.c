#define _XOPEN_SOURCE 500
#define UNUSED(x) (void)(x)

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>
#include "constants.h"

// parse the arguments for the put command
void parse_put_args(char *args, char **dir, char *files[], int *num_files, int *force) {
    char *arg = strtok(args, " ");
    while (arg != NULL) {
        if (strcmp(arg, "-f") == 0) {
            *force = 1; // set the force flag if the -f option is specified
        } else if (*dir == NULL) {
            *dir = arg; // set the directory if it has not been set yet
        } else {
            files[(*num_files)++] = arg; // add the file to the list of files to copy
        }
        arg = strtok(NULL, " ");
    }
}

// callback function for nftw to remove files and directories
int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    UNUSED(sb);
    UNUSED(typeflag);
    UNUSED(ftwbuf);
    
    int rv = remove(fpath); // remove the file or directory
    if(rv) {
        perror(fpath); // print an error message if the file or directory could not be removed
    }
    return rv;
}

// create a directory and copy files to it
void create_dir_and_copy_files(char *args) {
    char *dir = NULL;
    char *files[PUT_MAX_FILES] = { NULL };
    int num_files = 0;
    int force = 0;

    // Parse directory and file arguments
    parse_put_args(args, &dir, files, &num_files, &force);
    if (num_files == 0) {
        printf("No files specified!\n");
        return;
    }

    // Create directory
    if (mkdir(dir, 0777) == -1) { // create the directory with full permissions
        printf("Error creating \"%s\" directory!\n", dir);
        
        if(force) { // if the force flag is set, try to remove the directory and create it again
            printf("Forcing...\n");
            nftw(dir, unlink_cb, 64, FTW_DEPTH | FTW_PHYS); // remove the directory and its contents

            if(mkdir(dir, 0777) == -1) { // create the directory again
                printf("Error with force creating \"%s\" directory!\n", dir);
                return;
            }
        } else {
            return;
        }
    }

    printf("Directory \"%s\" created successfully\n", dir);

    // Copy files from current directory to new directory
    int successful_files = 0;

    for (int i = 0; i < num_files; i++) {
        char file_path[INPUT_ARGS_MAX_LENGTH];
        char new_file_path[INPUT_ARGS_MAX_LENGTH];
        FILE *fp;
        FILE *new_fp;
        char c;

        sprintf(file_path, "%s", files[i]); // get the path to the source file
        fp = fopen(file_path, "r"); // open the source file for reading

        if(fp == NULL) {
            printf("Error opening \"%s\" file! Skipping...\n", files[i]);
            continue;
        }

        sprintf(new_file_path, "%s/%s", dir, files[i]); // get the path to the destination file
        new_fp = fopen(new_file_path, "w"); // open the destination file for writing

        if(new_fp == NULL) {
            printf("Error creating \"%s\" file! Skipping...\n", files[i]);
            fclose(fp);
            continue;
        }

        while((c = fgetc(fp)) != EOF) { // copy the contents of the source file to the destination file
            fputc(c, new_fp);
        }

        successful_files++;
        fclose(fp);
        fclose(new_fp);
    }

    printf("%d files copied to directory: %s\n", successful_files, dir);
}