#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>

using namespace std;

int main() {
    // start timing
    auto c_start = clock();

    // counts of each sub-group of words
    vector<unordered_map<string, int>> counts(5);

    ifstream dictionary_file("dictionary.txt");

    if(!dictionary_file.is_open()) {
        cout << "dictionary.txt not found!";
        exit(0);
    }

    // loop through 
    string word;
    while(dictionary_file >> word) {
        // find the max length of the substring available given the word length and it must not be > 5
        auto length = word.length() < 5 ? word.length() : 5;

        // loop through each substring length available
        for(auto i = 2; i <= length; i++) {
            // algorithm to find the number of combinations available for each substring length
            // for example, if there was a substring of length 4, then the number of length 2 substrings = 3
            // the number of length 3 substrings = 2
            // the number of length 4 substrings = 1
            // so an equation can be derived of word length + 1 - substring length
            auto combinations = word.length() + 1 - i;

            // loop through total number of index offsets for the number of combinations
            for(int index = 0; index < combinations; index++) {
                // minus 2 so the length 2 substrings are at index 0
                // substring the word from the index of i length (substring length)
                // increase it's group count inside the substring length index in the vector
                counts[i-2][word.substr(index, i)]++;
            }
        }
    }

    dictionary_file.close();

    // finish clock
    auto c_end = clock();

    // loop through each substring length entry
    for(auto i = 2; i <= 5; i++) {
        // sort the unordered_map into a vector pair
        vector<pair<string, int>> vec;
        // use stl copy to copy the pairs into the vector pair
        copy(counts[i-2].begin(), counts[i-2].end(), back_inserter(vec));

        // using stl sort compare the order using lambda
        sort(vec.begin(), vec.end(), [](const pair<string, int>& l, const pair<string, int>& r) {
            return l.second > r.second;
        });

        cout << "Found " << vec.size() << " sub-sequences of length " << i << endl;

        // loop through the first 10 entries and print the substring and the count
        for(int e = 9; e >= 0; e--) {
            cout << vec[e].first << " " << vec[e].second << endl;
        }

        cout << endl;
    }

    // calculate CPU time and echo
    double cpu_time = ((double) (c_end - c_start)) / CLOCKS_PER_SEC;

    cout << "CPU time = " << setprecision(7) << cpu_time << " seconds" << endl;
    cout << endl;
}