#pragma once

#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

/**
 * Cleans a specific token by:
 * - Removing leading and trailing punctuation
 * - Converting to lowercase
 * If the token has no letters, returns the empty string.
 */
string cleanToken(string s) {
    // remove leading punctuation
    while (!s.empty() && ispunct(s.front())) {
        s.erase(s.begin());
    }

    // remove trailing punctuation
    while (!s.empty() && ispunct(s.back())) {
        s.pop_back();
    }

    // Return the empty string if the token does not contain at least one letter, as determined by isalpha.
    bool hasLetter = false;
    for (char c : s) {
        if (isalpha(c)) {
            hasLetter = true;
            break;
        }
    }
    if (!hasLetter) {
        return "";
    }

    // convert to lowercase
    for (char& c : s) {
        c = tolower(c);
    }

    // return the cleaned token
    return s;
}

/**
 * Returns a set of the cleaned tokens in the given text.
 */
set<string> gatherTokens(string text) {
    set<string> output;
    string current = "";

    for (char c : text) {
        // if space, clean current token
        if (isspace(c)) {
            // avoid another space
            if (!current.empty()) {
                string cleaned = cleanToken(current);
                if (!cleaned.empty()) {
                    output.insert(cleaned);
                }
                current = "";
            }
        } else {
            current += c;
        }
    }

    // clean the last token
    if (!current.empty()) {
        string cleaned = cleanToken(current);
        if (!cleaned.empty()) {
            output.insert(cleaned);
        }
    }

    return output;
}

/**
 * Builds the inverted index from the given data file. The existing contents
 * of `index` are left in place.
 */
#include <fstream> // Include the <fstream> header to resolve the incomplete type issue.

int buildIndex(string filename, map<string, set<string>>& index) {
    int processedCount = 0;

    // check if the file exist
    ifstream file(filename);
    if (!file) {
        cout << "Invalid filename." << endl;
        return 0;
    }

    // read the file
    string url;
    string line;
    while (getline(file, url)) {
        // read the next line
        getline(file, line);

        // process the line
        set<string> tokens = gatherTokens(line);
        for (const string& token : tokens) {
            index[token].insert(url);
        }

        // increment the processed count
        processedCount++;
    }

    return processedCount;
}

/**
 * Processes a single term in the search query, updating the results set.
 */
void processTerm(string term, map<string, set<string>>& index, set<string>& results) {
    if (term.empty()) {
        return;
    }

    // if term start with +
    if (term.front() == '+') {
        // clean operation first
        term.erase(term.begin());
        // clean term
        term = cleanToken(term);

        set<string> termSet = index[term];
        if (termSet.empty()) {
            results.clear();
        } else {
            set<string> intersection;
            set_intersection(
                results.begin(), results.end(),
                termSet.begin(), termSet.end(),
                inserter(intersection, intersection.begin())
            );
            results = intersection;
        }
    } else if (term.front() == '-') {
        term.erase(term.begin());
        term = cleanToken(term);

        set<string> termSet = index[term];
        if (!termSet.empty()) {
            set<string> difference;
            set_difference(
                results.begin(), results.end(), 
                termSet.begin(), termSet.end(), 
                inserter(difference, difference.begin())
            );
            results = difference;
        }
    } else {
        term = cleanToken(term);
        set<string> termSet = index[term];
        if (!termSet.empty()) {
            set<string> unionSet;
            set_union(
                results.begin(), results.end(), 
                termSet.begin(), termSet.end(), 
                inserter(unionSet, unionSet.begin())
            );
            results = unionSet;
        }
    }
}

/**
 * Runs a search query on the provided index, and returns a set of results.
 *
 * Search terms are processed using set operations (union, intersection, difference).
 */
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    set<string> results;

    string current = "";
    for (char c : sentence) {
        // if space, clean current token
        if (isspace(c)) {
            processTerm(current, index, results);
            current = "";
        } else {
            current += c;
        }
    }

    // process the last token
    if (!current.empty()) {
        processTerm(current, index, results);
    }
    
    return results;
}

/**
 * Runs the main command loop for the search program
 */
void searchEngine(string filename) {
    map<string, set<string>> index;
    int processedCount = buildIndex(filename, index);
    
    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << processedCount << " pages containing " << index.size() << " unique terms" << endl;

    string query;
    while (true) {
        cout << "\nEnter query sentence (press enter to quit): ";
        getline(cin, query);

        // If the query is the empty string, exit the command loop
        if (query == "") {
            break;
        }

        set<string> results = findQueryMatches(index, query);
        cout << "Found " << results.size() << " matching pages" << endl;
        for (const string& url : results) {
            cout << url << endl;
        }
    }

    cout << "Thank you for searching!" << endl;

}
