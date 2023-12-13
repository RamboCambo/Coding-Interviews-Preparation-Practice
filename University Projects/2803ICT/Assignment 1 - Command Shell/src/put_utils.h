#ifndef PUT_UTILS_H
#define PUT_UTILS_H

void parse_put_args(char *args, char **dir, char *files[], int *num_files, int *force);
void create_dir_and_copy_files(char *args);

#endif