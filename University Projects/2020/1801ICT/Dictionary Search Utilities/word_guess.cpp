// Include files go here
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <set>
#include <vector>

using namespace std;

// Class WordGuess goes here
class WordGuess
{
	// Private member functions and data
	vector<string> words_;
	int word_length_;
	int guesses_;
	bool solved_ = false;
	string current_word_;
	string letters_used_;
	// letters the players can choose from
	string letters_allowed_ = "abcdefghijklmnopqrstuvwxyz";

	// removes all occurances of a character from a string
	void remove_char(string& str, char c)
	{
		str.erase(remove(str.begin(), str.end(), c), str.end());
	}
	// Public
public:
	// Constructor (string, int, int) => reads file and creates data structures
	WordGuess(string file_name, int word_length, int guesses)
	{
		ifstream file(file_name);

		// populate words
		string word;
		while(file >> word)
		{
			if (word.length() == word_length) {
				words_.emplace_back(word);
			}
		}

		file.close();

		word_length_ = word_length;
		guesses_ = guesses;
	}

	// bool again() => returns true if guesses remaining and problem not solved
	bool again() { return !solved_ && guesses_ > 0; }

	// bool already_used(char) => returns true if letter already used
	bool already_used(char c) { return letters_used_.find(c) != string::npos; }

	// void add_letter(char) => adds letter to solution and updates data structures
	void add_letter(char c)
	{
		// holds easy to access count of the groups
		unordered_map<string, int> groups;
		// holds list of words
		unordered_map<string, vector<string>> group_words;

		// remove character from letters allowed
		remove_char(letters_allowed_, c);
		// add to used
		letters_used_.push_back(c);
		
		// loop through each word and replace each letter allwoed to be used with placeholder '-'
		for(auto word = words_.begin(); word != words_.end(); word++)
		{
			auto current = *word;
			for (auto allowed : letters_allowed_)
				replace(current.begin(), current.end(), allowed, '-');

			// increase count and add original word to the group that was derived from the placeholder format
			groups[current]++;
			group_words[current].emplace_back(*word);
		}

		// find max group
		string max_group = "";
		for(auto pair : groups)
		{
			// if it is not set
			if (max_group == "") {
				max_group = pair.first;
				continue;
			}
			// if there are more words in the next group vs the previous max group, then replace
			if (pair.second > groups[max_group])
				max_group = pair.first;
		}

		// move the objects to save memory copy
		words_ = std::move(group_words[max_group]);

		// set the current word place holder to the max group
		current_word_ = max_group;
		// decrease guesses
		guesses_--;
	}
	// string current_word() => returns the current user solution
	string current_word() { return current_word_; }
	// int guesses_left() => returns the current number of guesses remaining
	int guesses_left() { return guesses_; }
	// int words_left() => returns the current number of possible solutions
	int words_left() { return words_.size(); }
	// bool successful() => returns true if user guessed the word
	bool successful() { return solved_; }
	// string get_answer() => returns random word from the words remaining
	string get_answer()
	{
		return words_[rand() % words_.size()];
	}
};

int main(int argc, char* argv[])
{
	char letter;
	if (argc < 3)
	{
		cout << "Must enter word length and number of guesses on command line" << endl;
		exit(0);
	}
	WordGuess wg("dictionary.txt", atoi(argv[1]), atoi(argv[2]));
	while (wg.again())
	{
		cout << "Enter a letter: ";
		cin >> letter;
		if (!islower(letter))
			exit(0);
		if (wg.already_used(letter))
		{
			cout << "You have already tried " << letter << endl;
			continue;
		}
		wg.add_letter(letter);
		cout << wg.current_word() << ", Guesses left = " << wg.guesses_left()
			<< ", Words left = " << wg.words_left() << endl;
	}
	if (wg.successful())
		cout << "Good job!" << endl;
	else
		cout << "Sorry, the word was " << wg.get_answer() << endl;
	return 0;
}
