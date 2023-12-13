#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// check if a string is a number
static int is_number(char *token) {
    for (size_t i = 0; i < strlen(token); i++) {
        if (token[i] < '0' || token[i] > '9') {
            return 0;
        }
    }

    return 1;
}

// recursively calculate the result of the expression
static int calculate_next(char **token) {
    // get the next token from the string
    *token = strtok(NULL, " ");

    // if there are no more tokens, return 0
    if (*token == NULL) {
        return 0;
    }

    int num = 1;

    // check if the token is an operator
    if (strcmp(*token, "+") == 0) {
        num *= 1;
    } else if (strcmp(*token, "-") == 0) {
        num *= -1;
    } 
    // if it isn't an operator, then it's a number, so multiply by the number
    else {
        return 0;
    }

    // get the next token
    *token = strtok(NULL, " ");

    // check if the token is a number
    if (is_number(*token)) {
        num *= atoi(*token);
    } else {
        return 0;
    }

    // recursively calculate the result of the rest of the expression
    return num + calculate_next(token);
}

// calculate the result of the expression
int calculate(char *args) {
    // get the first token from the string
    char *token = strtok(args, " ");

    // if there are no tokens, return 0
    if (token == NULL) {
        return 0;
    }

    int num1 = 0;
    int num2 = 1;

    // check if the token is a number
    if (is_number(token)) {
        num1 = atoi(token);
    }

    // get the next token
    token = strtok(NULL, " ");

    // check if the token is an operator
    // then set num2 to 1 or -1
    if (strcmp(token, "+") == 0) {
        num2 *= 1;
    } else if (strcmp(token, "-") == 0) {
        num2 *= -1;
    } else {
        return num1;
    }

    // get the next token
    token = strtok(NULL, " ");

    // check if the token is a number
    if (is_number(token)) {
        num2 *= atoi(token);
    } else {
        return num1;
    }

    // recursively calculate the result of the rest of the expression
    return num1 + num2 + calculate_next(&token);
}