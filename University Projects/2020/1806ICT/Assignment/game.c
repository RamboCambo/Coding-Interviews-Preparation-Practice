// ReSharper disable CppDeprecatedEntity
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define MAX_COMMAND_SIZE 100

struct command
{
	char* command;
	char** arguments;
	int number_of_arguments;
};

struct tile
{
	int row;
	int column;
};

const char* welcome_message =
"Here are the rules:\n\n"
"- If you make matching pair, those tiles are deleted, and the board fills them in with new tiles. The board size can be between 3 and 50, inclusive.\n"
"- For each tile in a match, you earn 10 points.\n"
"- If you make more than one match on a tile swap, the swapped tile will be added to the points for each of left, up, right, or down you match.\n"
"- Try to reach the highest points you can so you can brag to your friends!\n"
"- A swap will only count if you get at least one row of 3 matching.\n"
"- There is a time limit of 60 seconds.\n\n"
"Here are the commands:\n\n"
"- new <board size> - This command either starts a new game or clears the current game and starts a new one. This will reset the board and the points. The board size can be between 3 and 50, inclusive. This cannot be run while in a game.\n"
"- exit - From the main input screen, this will exit the game entirely. This cannot be run while in a game.\n"
"- load [<name of board>] - This command will load a saved game from the game directory. It will attempt to load board.txt or the board name specified. If it does exist, the game will be set to the specified board and points in the file. If invalid data is found, the game will print, \"Error! Invalid game board data!\". If the file does not exist, the game will print, \"Error! No board file found!\" This cannot be run while in a game.\n\n"
"- swap (<row>,<column>) with (<row>,<column>) - This command can be run to swap two tiles. The tiles are represented by a row and column integer, defined by starting the game. If the tile location specified isn't inside the board, the game will print the error message, \"Error! Invalid tile position!\" This command can only be run when the game is on process.\n"
"- quit - This command finishes the game and returns to the start screen. It calculates the final points and shows the time played. This can only be run while in a game.\n"
"- save [<name of board>] - This command will save a current game board into a file. The file will be called board.txt by default or the optional name specified as an argument. Please note that the argument cannot have any spaces, or the board will not be named as expected. The files will be located within the save directory as the game is run from. This command can only be run when a game is in process.\n"
"- refresh - This command will refresh the board by repopulating with new values. This can only be run when in the game.\n"
"- help - This command will show the rules and commands again. This can be run at any time.\n\n";

// major game variables
char characters[5] = {'%', '#', '&', '$', '@'};
int board_size;
char* board;
int points = 0;
float score = 0;
time_t start_time;
time_t total_time = 0;
// is set after every command is done
char* last_command_message;

// define the game functions prototypes

// game display
void display_welcome(void);
char* string_to_lower(char* input);

// game input
struct command get_user_input(void);
bool check_is_number(char* input);
bool check_is_alphanumeric(char* input);
void free_input_memory(struct command input);

// board management
char* copy_board(void);
void populate_board(void);
void swap(char* tile_input_1, char* tile_input_2);
int count_tiles(char* swapped_board, char tile_char, struct tile tile, struct tile tile_difference, int count);
int count_all_tiles_and_remove(char* swapped_board, char tile_char, struct tile tile);
bool is_tile_in_board(struct tile tile);
void assign_board_memory(void);
void free_board_memory(void);
char get_char(char* current_board, struct tile tile);
char get_new_tile(struct tile tile);
char pick_random_char(char available_chars[], int number_of_chars);
void set_tile(char* current_board, struct tile tile, char new_char);
void shift_tiles(char* current_board);
void shift_swap_tiles(char* current_board, struct tile tile, int row_difference);

// board display
void display_board(void);
void display_time_score(void);

// game management
void calculate_score(void);
void calculate_total_time(void);
void quit(void);
void save(char* file_name);
void new_game(int new_board_size);
void load(char* file_name);
void run_game(void);

int main()
{
	// Show the welcome screen
	display_welcome();

	// start command loop
	while (1)
	{
		printf("Type a command here. For more commands, type help: ");
		struct command user_input = get_user_input();

		// main screen commands
		if (strcmp(user_input.command, "new") == 0)
		{
			// ensure a argument is passed in the size placeholder
			if (user_input.arguments != NULL) {
				if (!check_is_number(user_input.arguments[0]))
				{
					printf("The argument contained non-numeric characters!\n\n");
					continue;
				}
				
				int new_board_size = atoi(user_input.arguments[0]);
				new_game(new_board_size);
			}
			else
			{
				printf("No game size was passed as an argument!\n\n");
			}
		}

		// exit the game
		else if (strcmp(user_input.command, "exit") == 0)
		{
			free_input_memory(user_input);
			return 0;
		}

		// load a game from the directory the game is run from
		else if (strcmp(user_input.command, "load") == 0)
		{
			// ensure an argument is passed for the board name
			if (user_input.arguments != NULL) {
				load(user_input.arguments[0]);
			}
			else
			{
				printf("No game board name was given!\n\n");
			}
		}

		else if (strcmp(user_input.command, "help") == 0 || strcmp(user_input.command, "help?") == 0)
		{
			display_welcome();
		}
		
		// if a command which is unknown has been executed
		else
		{
			printf("Invalid command! Type help to view all available commands.\n\n");
		}

		free_input_memory(user_input);
	}
}

// game display functions

void display_welcome(void)
{
	// prints the welcome message defined above
	printf("%s", welcome_message);
}

char* string_to_lower(char* input)
{
	// for each letter, replace it with the lower variant and return the concatenated string
	for (int i = 0; i < (int)strlen(input); ++i)
	{
		input[i] = (char)tolower(input[i]);
	}

	return input;
}

// game input functions

struct command get_user_input(void)
{
	// setup the user input object
	struct command user_input;

	// get the input the user typed
	char input[100];
	fgets(input, sizeof(input), stdin);

	// initialise the delimiters the string is tokenised by, being space and newline
	char* delimiters = " \n";

	// get the first token
	char* token = strtok(input, delimiters);

	// assign memory to the command
	user_input.command = malloc(sizeof(char) * MAX_COMMAND_SIZE);

	// copy the first token to the command and convert to lowercase
	strcpy(user_input.command, token);
	user_input.command = string_to_lower(user_input.command);

	// initialise the arguments to nothing
	user_input.arguments = NULL;

	// create loop to get any further arguments passed by passing NULL so it uses the previous input given
	int index = 0;
	token = strtok(NULL, delimiters);
	while (token != NULL)
	{
		// reallocate the memory of arguments to be big enough for the previous, if any, arguments plus the new one
		user_input.arguments = realloc(user_input.arguments, sizeof(char*) * (index + 1));

		// if memory allocation fails, exit the program with error message
		if (user_input.arguments == NULL)
		{
			printf("Error! Could not allocate enough memory for the command arguments!\n\n");
			exit(0);
		}

		// allocate and copy the current token to the argument stack
		user_input.arguments[index] = malloc(sizeof(char) * MAX_COMMAND_SIZE);
		strcpy(user_input.arguments[index], token);

		// get next argument and loop again
		token = strtok(NULL, delimiters);

		// increase the index for the next argument position
		++index;
	}

	free(token);

	// set the number of arguments
	user_input.number_of_arguments = index;

	return user_input;
}

bool check_is_number(char* input)
{
	int input_length = (int)strlen(input);

	// loop through the characters
	for (int i = 0; i < input_length; ++i)
	{
		// return false if it isn't a digit
		if (!isdigit(input[i]))
		{
			return false;
		}
	}

	// if it got here, all the characters were digits
	return true;
}

bool check_is_alphanumeric(char* input)
{
	const int input_length = (int)strlen(input);

	// loop through all the characters
	for (int i = 0; i < input_length; ++i)
	{
		// if the character isn't alphanumeric, return false
		if (!isalnum(input[i]))
			return false;
	}

	// otherwise if it hasn't returned null, all the characters were alphanumeric
	return true;
}

void free_input_memory(struct command input)
{
	free(input.command);

	for (int i = 0; i < input.number_of_arguments; ++i)
	{
		free(input.arguments[i]);
	}

	free(input.arguments);
}

// board management functions

char* copy_board(void)
{
	// allocate the memory needed
	char* swapped_board = malloc(sizeof(char) * (int)(pow(board_size, 2) + 1));

	strcpy(swapped_board, board);

	// return the copied board
	return swapped_board;
}

void populate_board(void)
{
	assign_board_memory();

	// seed the time function with the seconds since Jan 1 1970
	srand((unsigned)time(NULL));

	// loop through the board positions and get a valid char for each position
	for (int row = 0; row < board_size; ++row)
	{
		for (int column = 0; column < board_size; ++column)
		{
			struct tile tile = { row, column };

			set_tile(board, tile, get_new_tile(tile));
		}
	}
}

void swap(char* tile_input_1, char* tile_input_2)
{
	// sets the initial positions to -1 so if one is not set, it will fail the inside board test
	struct tile tile1 = { -1, -1 };
	struct tile tile2 = { -1, -1 };

	sscanf(tile_input_1, "(%d,%d)", &tile1.row, &tile1.column);
	sscanf(tile_input_2, "(%d,%d)", &tile2.row, &tile2.column);

	// convert to index form so they can be selected in an array by subtracting 1
	--tile1.row;
	--tile1.column;
	--tile2.row;
	--tile2.column;

	// check both tiles are inside the board
	const bool both_inside_board = is_tile_in_board(tile1) && is_tile_in_board(tile2);

	if (!both_inside_board)
	{
		last_command_message = "Error! Both tiles were not inside the board bounds!\n";
		return;
	}

	// find the absolute distance between the tiles on the horizontal and vertical axises
	// and find whether one of them is equal to 1 (they are adjacent on one of the axises, but not both)
	bool tiles_are_adjacent = (abs(tile1.row - tile2.row) == 1) != (abs(tile1.column - tile2.column) == 1);

	if (!tiles_are_adjacent)
	{
		last_command_message = "Error! Tiles are not adjacent!\n";
		return;
	}

	// copy the board so it doesn't affect the laying board
	char* swapped_board = copy_board();

	// get the characters that the program needs to find matches for and swap them
	char tile1_char = get_char(board, tile2);
	char tile2_char = get_char(board, tile1);

	// swap tiles
	set_tile(swapped_board, tile1, tile1_char);
	set_tile(swapped_board, tile2, tile2_char);

	// count tiles for each tile swap recursively and remove them
	int tile1_score = count_all_tiles_and_remove(swapped_board, tile1_char, tile1);
	int tile2_score = count_all_tiles_and_remove(swapped_board, tile2_char, tile2);

	// check if neither of them have found any matches
	if (tile1_score < 30 && tile2_score < 30)
	{
		last_command_message = "No suitable matches were found from the swap!\n";
	}
	else
	{
		last_command_message = "Success! The tiles were swapped.\n";

		// add swap scores to the board
		points += tile1_score + tile2_score;
		// clear board memory because the swapped board is replacing it
		free_board_memory();

		// go through and move down tiles here and calculate cascading permutations
		shift_tiles(swapped_board);

		// define the cascading points
		int cascading_score;

		// run the cascading check at least once
		do
		{
			cascading_score = 0;

			// loop through each piece on the board to see if there are any matches caused
			// by the swap
			for (int row = 0; row < board_size; ++row)
			{
				for (int column = 0; column < board_size; ++column)
				{
					// convert the row and column to a tile object
					struct tile tile = { row, column };

					// count the points for the tile and remove tiles if matches are found
					int tile_score = count_all_tiles_and_remove(swapped_board, get_char(swapped_board, tile), tile);

					// add the points to the current cascading points
					cascading_score += tile_score;
				}
			}

			// add the cascading points to the total points
			points += cascading_score;

			// shift all the tiles and add new ones
			shift_tiles(swapped_board);

			// notify that a cascade was applied too
			if (cascading_score > 0)
				last_command_message = "Success! The tiles were swapped, and cascading matches were found too!\n";

			// check if the cascading found any matches, and if so run cascading check again
		} while (cascading_score != 0);

		// set the pointer of the board to the swapped board
		board = swapped_board;
	}
}

int count_tiles(char* swapped_board, char tile_char, struct tile tile, struct tile tile_difference, int count)
{
	// create the next tile to check if it matches by adding the difference
	struct tile next_tile =
	{
		tile.row + tile_difference.row,
		tile.column + tile_difference.column
	};

	// if the tile isn't inside the board, return the current count of matches
	// if the tile isn't inside the board, return the current count of matches
	if (!is_tile_in_board(next_tile))
	{
		return count;
	}

	// if the next tile's character matches, then keep searching for more matches
	if (get_char(swapped_board, next_tile) == tile_char)
	{
		count = count_tiles(swapped_board, tile_char, next_tile, tile_difference, count + 1);
	}

	// return the current count of matches found
	return count;
}

int count_all_tiles_and_remove(char* swapped_board, char tile_char, struct tile tile)
{
	// prevents cascading effect counting already removed tiles
	if (tile_char == '.')
		return 0;

	// count all the matches to the left, right, top, and bottom of the piece in question by modifying and passing a tile difference to be applied
	struct tile tile_difference = { 0, -1 };
	int tile_left = count_tiles(swapped_board, tile_char, tile, tile_difference, 0);

	tile_difference.column = 1;
	int tile_right = count_tiles(swapped_board, tile_char, tile, tile_difference, 0);

	tile_difference.row = -1;
	tile_difference.column = 0;
	int tile_up = count_tiles(swapped_board, tile_char, tile, tile_difference, 0);

	tile_difference.row = 1;
	int tile_down = count_tiles(swapped_board, tile_char, tile, tile_difference, 0);

	// add up all the matches on each axis
	int tile_horizontal = tile_left + tile_right;
	int tile_vertical = tile_up + tile_down;

	// if the amount of tiles counted on the horizontal axis was less than 2 (not including the target tile), then no matches were found
	if (tile_horizontal < 2)
	{
		tile_horizontal = 0;
	}
	// remove the tiles because there was a match
	else
	{
		// set the target tile to removed placeholder
		set_tile(swapped_board, tile, '.');

		struct tile removed_tile = { tile.row, tile.column };

		// replacing all the tiles at the corresponding locations for left and right
		for (int column = 1; column <= tile_left; ++column)
		{
			removed_tile.column = tile.column - column;
			set_tile(swapped_board, removed_tile, '.');
		}

		for (int column = 1; column <= tile_right; ++column)
		{
			removed_tile.column = tile.column + column;
			set_tile(swapped_board, removed_tile, '.');
		}
	}

	// if the amount of tiles counted on the vertical axis was less than 2 (not including the target tile), then no matches were found
	if (tile_vertical < 2)
	{
		tile_vertical = 0;
	}

	// remove the tiles because there was a match
	else
	{
		// set the target tile to removed placeholder
		set_tile(swapped_board, tile, '.');

		struct tile removed_tile = { tile.row, tile.column };

		// replacing all the tiles at the corresponding locations for up and down
		for (int row = 1; row <= tile_up; ++row)
		{
			removed_tile.row = tile.row - row;
			set_tile(swapped_board, removed_tile, '.');
		}

		for (int row = 1; row <= tile_down; ++row)
		{
			removed_tile.row = tile.row + row;
			set_tile(swapped_board, removed_tile, '.');
		}
	}

	// if one of the axis have a match, then return them both added together
	// it doesn't matter because they're set to zero if they don't meet the criteria for minimum match length (2, not including target tile)
	if (tile_horizontal >= 2 || tile_vertical >= 2)
	{
		// + 1 is for the root tile of the swap
		return (tile_horizontal + tile_vertical + 1) * 10;
	}

	// return there were no matches found
	return 0;
}

bool is_tile_in_board(struct tile tile)
{
	return tile.row >= 0 && tile.row < board_size&& tile.column >= 0 && tile.column < board_size;
}

void assign_board_memory(void)
{
	// create the total memory needed, plus one for the terminator
	board = malloc(sizeof(char*) * (int)(pow(board_size, 2) + 1));

	// add the terminator
	board[(int)pow(board_size, 2)] = '\0';
}

void free_board_memory(void)
{
	// check if the board is null, then free the board memory for all columns, then the rows
	if (board != NULL)
	{
		free(board);
		// make sure the board is seen as freed memory
		board = NULL;
	}
}

char get_char(char* current_board, struct tile tile)
{
	return current_board[tile.row * board_size + tile.column];
}

char get_new_tile(struct tile tile)
{
	// there can be at most two illegal characters, so the x is a placeholder
	char illegal_chars[2] = { 'x', 'x' };

	// If it is greater or equal to 2 places from the row = 0, character selection matters
	if (tile.row >= 2)
	{
		// get the two characters above
		struct tile tile_up = { tile.row - 1, tile.column };

		char a = get_char(board, tile_up);

		--tile_up.row;
		char b = get_char(board, tile_up);

		// if the two chars are equal, then that character can't be used
		if (a == b)
		{
			illegal_chars[0] = a;
		}
	}

	// If it is greater or equal to 2 places from the column = 0, character selection matters
	if (tile.column >= 2)
	{
		// get the two characters above
		struct tile tile_left = { tile.row, tile.column - 1 };

		char a = get_char(board, tile_left);

		--tile_left.column;
		char b = get_char(board, tile_left);

		// if the two chars are equal, then that character can't be used
		if (a == b)
		{
			illegal_chars[1] = a;
		}
	}

	// find how many legal characters there are so it will only pick from that many characters
	int legal_chars_count = 0;
	char legal_chars[5];

	// loop through available chars to determine if they're illegal
	for (int i = 0; i < 5; ++i)
	{
		bool is_legal_char = characters[i] != illegal_chars[0] && characters[i] != illegal_chars[1];

		if (!is_legal_char)
		{
			continue;
		}

		legal_chars[legal_chars_count] = characters[i];
		++legal_chars_count;
	}

	return pick_random_char(legal_chars, legal_chars_count);
}

char pick_random_char(char available_chars[], int number_of_chars)
{
	// since it uses mod, it will always return the remainder, which will be up to number_of_chars - 1
	return available_chars[rand() % number_of_chars];
}

void set_tile(char* current_board, struct tile tile, char new_char)
{
	current_board[tile.row * board_size + tile.column] = new_char;
}

void shift_tiles(char* current_board)
{
	// loop through each column to cascade
	for (int column = 0; column < board_size; ++column)
	{
		// loop through the rows from the bottom up
		for (int row = board_size - 1; row >= 0; --row)
		{
			struct tile tile = { row, column };

			char current_char = get_char(current_board, tile);

			// if the current char has been removed cascade the tiles in that column downward from that row upwards
			if (current_char == '.')
			{
				shift_swap_tiles(current_board, tile, 1);
			}
		}
	}
}

void shift_swap_tiles(char* current_board, struct tile tile, int row_difference)
{
	// if the next tile it is trying to swap with is outside the board, then pick a random character
	if (tile.row - row_difference < 0)
	{
		set_tile(current_board, tile, pick_random_char(characters, 5));
		return;
	}

	// get the next tile above it and it's character
	struct tile next_tile = { tile.row - row_difference, tile.column };
	char next_char = get_char(current_board, next_tile);

	// if the next character has not been removed, then swap the characters so
	// the tile which hasn't been removed shifts down and the removed tile replaces it
	if (next_char != '.')
	{
		set_tile(current_board, tile, next_char);
		set_tile(current_board, next_tile, '.');
		return;
	}

	// keep searching for the next tile it can swap with by calling this function again
	shift_swap_tiles(current_board, tile, ++row_difference);
}

// board display

void display_board(void)
{
	// clear the screen
	system("cls");

	display_time_score();

	// use log10 + 2 to find max number of digits to pad by
	int pad_length = (int)log10(board_size) + 1;

	// print the top numbers' padding to bring in line with numbers on the left
	printf("%*c ", pad_length, ' ');

	// loop through all the board positions and print 
	for (int row = -1; row < board_size; ++row)
	{
		if (row != -1)
		{
			// increase row and column by 1 to make side numbers more human-readable
			printf("%*i ", pad_length, row + 1);
		}

		for (int column = 0; column < board_size; ++column)
		{
			if (row == -1)
			{
				printf("%*i ", pad_length, column + 1);
				continue;
			}

			struct tile tile = { row, column };

			printf("%*c ", pad_length, get_char(board, tile));
		}

		printf("\n");
	}

	printf("\n");

	if (last_command_message != NULL)
	{
		printf("%s\n", last_command_message);
	}
}

void display_time_score(void)
{
	printf("Points: %d\n", points);
	printf("Score: %.2f\n", score);
	printf("Time played: %ld seconds\n\n", (long)total_time);
}

// game management

void calculate_score(void)
{
	calculate_total_time();
	score = (float)points / (float)total_time;
}

void calculate_total_time(void)
{
	// find total time played by finding difference between start time and now
	total_time = time(0) - start_time;

	// prevent division by zero
	if (total_time == 0)
		total_time = 1;
}

void quit(void)
{
	free_board_memory();

	// total time played in seconds
	calculate_score();
	display_time_score();
}

void save(char* file_name)
{
	calculate_score();

	if (!check_is_alphanumeric(file_name))
	{
		last_command_message = "The file name can only contain letters and numbers!\n";
		return;
	}

	FILE* file = fopen(strcat(file_name, ".txt"), "w");

	// board exists - assuming only the game has access to these so they should be in the right format
	if (file != NULL)
	{
		calculate_total_time();
		// get values
		fprintf(file, "%d\n", points);
		fprintf(file, "%ld\n", (long)total_time);
		fprintf(file, "%d\n", board_size);

		// print entire board to single line
		fprintf(file, "%s", board);

		// close the file
		fclose(file);

		last_command_message = "The game has successfully been saved!\n";

		return;
	}

	last_command_message = "The game was unsuccessful in being saved!\n";
}

void new_game(int new_board_size)
{
	free_board_memory();

	if (new_board_size < 3 || new_board_size > 50)
	{
		printf("Invalid board size! It cannot be less than 3 or greater than 50!\n\n");
		return;
	}

	// set the board size to the new one
	board_size = new_board_size;

	// print new line for some extra vertical space
	printf("\n");

	// populate the board then start the game
	populate_board();

	// set the initial scores and game start time
	points = 0;
	score = 0;
	start_time = time(0);
	
	run_game();
}

void load(char* file_name)
{
	// add .txt to filename given
	// look for file in game directory with the board name given
	FILE* file = fopen(strcat(file_name, ".txt"), "r");

	// board exists - assuming only the game has access to these so they should be in the right format
	if (file != NULL)
	{
		printf("Success! The board exists! Loading now...\n");

		// load each line to corresponding setting
		char loaded_score[100];
		char loaded_total_time[100];
		char loaded_board_size[100];

		// get values
		fgets(loaded_score, sizeof(loaded_score), file);
		fgets(loaded_total_time, sizeof(loaded_total_time), file);
		fgets(loaded_board_size, sizeof(loaded_board_size), file);

		// convert values and load into global variables
		points = atoi(loaded_score);

		// using the total time, we can adjust by subtracting it from the time now to make
		// it appear as if we started the game x amount of seconds ago
		start_time = time(0) - atol(loaded_total_time);
		calculate_total_time();

		board_size = atoi(loaded_board_size);

		// now that we've defined the board size, we can assign memory to it
		assign_board_memory();

		// copy the board directly into the board object
		fgets(board, (int) pow(board_size, 2) + 1, file);

		// close the file
		fclose(file);

		last_command_message = "The game has successfully been loaded!\n";

		// start the game until it is finished and exited and the control will flow back to the main loop
		run_game();

		// once run game has finished, control will return to the main method
		return;
	}

	printf("The game was unsuccessful in being loaded!\n\n");
}

void run_game(void)
{
	// board loop
	while (1) {
		// calculate the points and display current board state
		calculate_score();
		display_board();

		// get the user input
		printf("Type command here. For more commands, type help: ");
		struct command user_input = get_user_input();

		// swap the board at tiles specified
		if (strcmp(user_input.command, "swap") == 0)
		{
			// check if they gave the correct amount of arguments
			if(user_input.number_of_arguments != 3)
			{
				last_command_message = "Too many or too little arguments were supplied for swapping tiles!\n";
				continue;
			}

			if (strcmp(user_input.arguments[1], "with") == 0)
			{
				// swap the board at the tiles if the conditions were met
				swap(user_input.arguments[0], user_input.arguments[2]);
			}
			else
			{
				last_command_message = "The command format for swap was not specified correctly.";
			}
		}

		// quit the game and return to the main menu
		else if (strcmp(user_input.command, "quit") == 0)
		{
			free_input_memory(user_input);
			quit();
			// return to the main screen
			break;
		}

		// save the game with a file name
		else if (strcmp(user_input.command, "save") == 0)
		{
			// check a name was given for the board
			if (user_input.arguments == NULL)
			{
				last_command_message = "No arguments were supplied for saving the game!\n";
				continue;
			}

			// save the board to disk
			save(user_input.arguments[0]);
		}

		// if the user can't find any new combinations, let them refresh the board
		else if (strcmp(user_input.command, "refresh") == 0)
		{
			// clear the board memory and repopulate it
			free_board_memory();
			populate_board();
		}

		else if (strcmp(user_input.command, "help") == 0 || strcmp(user_input.command, "help?") == 0)
		{
			last_command_message = (char*) welcome_message;
		}

		// no valid command was specified
		else
		{
			last_command_message = "Invalid command! Please type help for available commands!\n";
		}

		free_input_memory(user_input);
	}
}