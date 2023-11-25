#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct command
{
	char* command;
	char** arguments;
};

struct command get_user_input(int number_of_arguments)
{
	struct command user_input;
	char input[100];

	fgets(input, sizeof(input), stdin);

	// characters the string is split by
	char* delimiters = " \n";

	char* token = strtok(input, delimiters);
	user_input.command = malloc(sizeof(char) * strlen(token));

	strcpy(user_input.command, token);

	user_input.arguments = NULL;
	
	int index = 0;
	token = strtok(NULL, delimiters);
	while(token != NULL)
	{
		user_input.arguments = realloc(user_input.arguments, sizeof(char) * (index + 1));
		user_input.arguments[index] = token;
		
		token = strtok(NULL, delimiters);

		++index;
	}
	
	return user_input;
}

int main()
{
	struct command user_input = get_user_input(2);

	printf("%s", user_input.command);

	if (user_input.arguments != NULL) {
		for (int i = 0; i < sizeof(user_input.arguments) / sizeof(char*); ++i)
		{
			printf("%s", user_input.arguments[i]);
		}
	}
	
	return 0;
}