#include <stdio.h>
#include <time.h>

// print the current time and date
void print_time() {
    time_t t = time(NULL); // get the current time
    struct tm *tm = localtime(&t); // convert the time to a struct tm
    printf("Current time: %02d:%02d:%02d\n", tm->tm_hour, tm->tm_min, tm->tm_sec); // print the current time
    printf("Current date: %02d/%02d/%04d\n", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900); // print the current date
}