#include <stdio.h>
#include <sys/utsname.h>

// print system information
void print_sys_info() {
    struct utsname uname_data; // struct to hold system information

    // get system information using the uname function
    if (uname(&uname_data) != -1) {
        printf("OS name: %s\n", uname_data.sysname); // print the operating system name
        printf("OS version: %s\n", uname_data.release); // print the operating system version
        printf("CPU type: %s\n", uname_data.machine); // print the CPU type
    } else {
        perror("uname() error"); // print an error message if the system information could not be obtained
    }
}