#include "input_validation.h"

int is_input_numeric(char *input)
{
    int is_numeric = 1;
    size_t length = strlen(input);

    if (length == 0)
    {
        return 0;
    }

    for (size_t i = 0; i < length; i++)
    {
        if (!isdigit(input[i]))
        {
            is_numeric = 0;
            break;
        }
    }

    return is_numeric;
}