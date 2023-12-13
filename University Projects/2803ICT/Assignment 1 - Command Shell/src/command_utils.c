#include <string.h>

// clear the input buffers
void clear_input(char *command, char *args) {
    // set the memory of the command buffer to 0
    memset(command, 0, sizeof(*command));
    // set the memory of the args buffer to 0
    memset(args, 0, sizeof(*args));
}