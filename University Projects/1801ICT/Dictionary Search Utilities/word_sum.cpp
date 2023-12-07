// Include files go here
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <cmath>

using namespace std;

// Class WordSum goes here
class WordSum {
private:
    int letters_used = 0;
    string letters_;
    vector<string> terms_;
    string result_;
    int attempts_ = 0;
    unordered_map<char, int> correct_mappings;

    // alternative to pow function
    int multiplier(int power) {
        int num = 1;

        // keep multiplying 1 by 10 until it reaches the correct exponent
        for(int i = 0; i < power; i++)
            num *= 10;
        
        // return this number
        return num;
    }

    string distinct(const vector<string>& terms, const string& result);
    bool find_permutations(string& letters, unordered_map<char, int>& start, unordered_map<char, int>& mappings, unordered_map<char, bool>& used, vector<bool>& used_numbers);
    bool evaluate(unordered_map<char, int>& mappings);
public:
    WordSum(vector<string> terms, string result);
    friend ostream& operator<<(ostream& stream, WordSum& word_sum);
};
// Private member functions and data used to solve problem
// Public
// Constructor (vector<string>, string) => creates data structures and solves problems
// Overloaded operator<<

WordSum::WordSum(vector<string> terms, string result) {
    terms_ = terms;
    result_ = result;
    
    // get unique list of letters from the terms and result combined
    auto letters = distinct(terms, result);

    letters_ = letters;

    // map of starting letter's number
    unordered_map<char, int> start;
    // map of active numbers for each letter
    unordered_map<char, int> mappings;
    // map of chars used
    unordered_map<char, bool> used;

    // can only be a maximum of 10 different letters because letters are unique
    // so this can always guarantee you'll be able to reference whether a letter index is being used or not
    vector<bool> used_numbers(10);

    // go through each letter and set the start to 0
    for(auto letter : letters) {
        start[letter] = 0;
    }

    // set the start of each term and result to 1
    for(auto term : terms) {
        start[term[0]] = 1;
    }

    // set the start of the first letter in the result to 1
    start[result[0]] = 1;

    // empty used number vector because no numbers have been used
    // start permutations
    find_permutations(letters, start, mappings, used, used_numbers);
}

bool WordSum::find_permutations(string& letters, unordered_map<char, int>& start, unordered_map<char, int>& mappings, unordered_map<char, bool>& used, vector<bool>& used_numbers) {
    // check all letters are used
    if(letters_used == letters.length()) {
        // return evaluated value
        // the previous call will determine if it was a successful evaluation or not
        return evaluate(mappings);
    }
    else {
        // loop through each letter
        // get the current letter being dealt with
        // letters_used will be 0 at the start so it will go the the first index and increase
        auto letter = letters[letters_used++];

        // set the letter being used to true
        used[letter] = true;
        // go through each number not in use from start
        // loop through each possible number it can be
        for(auto num = start[letter]; num <= 9; num++) {
            // if the number is alrready in use, skip it
            if(used_numbers[num])
                continue;

            // if it hasn't been used, set it to be used
            used_numbers[num] = true;

            // set the mapping to be the number
            mappings[letter] = num;
            // find the permutations
            auto correct = find_permutations(letters, start, mappings, used, used_numbers);
            
            // if the eventual evaluation was correct, then return true up here
            if(correct)
                return true;

            // no need to clear the mapping because the letters_used determines if it has reached an evaulation stage, so the mappings get overridden before then

            // the number is no longer in use, so set its use to false
            used_numbers[num] = false;
        }
        // we've finished with this letter because the recursive calls done with are are over
        used[letter] = false;

        // we're now using one less letter, so decrease
        letters_used--;

        // return false / break the loop because we will get all the permutations without having to go through the whole order again
        // return false for this sub-tree of combinations because if it has reached here, then it never returned true
        return false;
    }
}

bool WordSum::evaluate(unordered_map<char, int>& mappings) {
    // increase the attempts
    attempts_++;

    int sum = 0;
    int result = 0;

    // calculate the result from the mappings multiplied by the exponent
    auto exponent = result_.length();
    for(auto letter : result_) {

        result += mappings[letter] * multiplier(exponent--);
    }

    // calculate and add the terms from the mappings multipled by the exponent
    for(auto term : terms_) {
        exponent = term.length();

        for(auto letter : term) {
            sum += mappings[letter] * multiplier(exponent--);
        }
    }

    // if the mappings calculations aligned, then the it was the correct mappings, so return true
    if(sum == result) {
        // copy the correct mappings
        correct_mappings = mappings;
        return true;
    }
    // return false otherwise
    return false;
}

ostream& operator<<(ostream& stream, WordSum& word_sum) {
    stream << "Problem: ";

    // print the terms and +'s separating them with an = before the result
    stream << word_sum.terms_[0];
    for(auto term = ++word_sum.terms_.begin(); term != word_sum.terms_.end(); term++) {
        stream << " + " << *term;
    }
    stream << " = " << word_sum.result_ << endl;
    
    // print each letter's mapping with the attempts afterwards
    for(auto letter : word_sum.letters_) {
        stream << letter << " = " << word_sum.correct_mappings[letter] << ", ";
    }

    stream << "Attempts = " << word_sum.attempts_ << endl;

    return stream;
}

string WordSum::distinct(const vector<string>& terms, const string& result) {
    string letters;
    
    // add all the letters to a string
    for(auto& term : terms)
        letters += term;
    letters += result;
    
    string distinct_letters;

    // loop through each letter in the combined letters, and check the count of that letter in the distinct letters
    // if it is equal to 0, then you can add
    for(auto letter : letters) {
        if(count(distinct_letters.begin(), distinct_letters.end(), letter) == 0) {
            distinct_letters += letter;
        }
    }

    // return the distinct letters
    return distinct_letters;
}

int main()
{
    WordSum ws1({"SEND", "MORE"}, "MONEY");
    cout << ws1 << endl;
    WordSum ws2({"EARTH", "AIR", "FIRE", "WATER"}, "NATURE");
    cout << ws2 << endl;
    WordSum ws3({"SATURN", "URANUS", "NEPTUNE", "PLUTO"}, "PLANETS");
    cout << ws3 << endl;
     return 0;
}