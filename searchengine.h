#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <limits> // Required for numeric_limits
using namespace std;

#include "hashtable.h"

// -------------------- helper normalize --------------------
static string toLowerString(string s) {
    for (int i = 0; i < (int)s.size(); i++)
        s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

static bool containsIgnoreCase(const string& haystack, const string& needle) {
    string h = toLowerString(haystack);
    string n = toLowerString(needle);
    return (h.find(n) != string::npos);
}

class SearchEngine {
private:
    HashTable& catalog;

public:
    SearchEngine(HashTable& c) : catalog(c) {}

    // --------------------------------------------------------
    // INTERACTIVE SEARCH (For "Search Book" Menu)
    // --------------------------------------------------------
    Book* findBookInteractive() {
        cout << "\nEnter ISBN / Title / Author: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
        string query;
        getline(cin, query);

        query = toLowerString(query);

        // 1) Try ISBN directly first (exact)
        Book* byIsbn = catalog.search(query);
        if (byIsbn) return byIsbn;

        // 2) Search title/author contains
        auto list = catalog.getAllBooks();

        Book* matches[50];
        int matchCount = 0;

        // First Pass: Look for Partial Matches
        for (auto b : list) {
            if (containsIgnoreCase(b->title, query) || containsIgnoreCase(b->author, query)) {
                matches[matchCount++] = b;
                if (matchCount >= 50) break;
            }
        }

        // 3) If no partial matches, Look for Suggestions (Fuzzy)
        bool isSuggestion = false;
        if (matchCount == 0) {
            for (auto b : list) {
                if (!query.empty() && !b->title.empty() &&
                    tolower((unsigned char)b->title[0]) == tolower((unsigned char)query[0])) {
                    matches[matchCount++] = b;
                    if (matchCount >= 5) break; // Limit suggestions to 5
                }
            }
            if (matchCount > 0) isSuggestion = true;
        }

        // If STILL nothing, give up
        if (matchCount == 0) {
            cout << "\nNo matching books found.\n";
            return nullptr;
        }

        // Print Results
        if (isSuggestion) 
            cout << "\nNo exact match. Did you mean:\n";
        else 
            cout << "\nMatches found:\n";

        for (int i = 0; i < matchCount; i++) {
            cout << i + 1 << ". " << matches[i]->title
                 << " (ISBN: " << matches[i]->isbn << ") by "
                 << matches[i]->author << "\n";
        }

        // Auto-select if it's a perfect single match (not a suggestion)
        if (matchCount == 1 && !isSuggestion) return matches[0];

        // Selection Logic
        cout << "Choose 1-" << matchCount << " (0 to cancel): ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > matchCount) return nullptr;
        return matches[choice - 1];
    }

    // --------------------------------------------------------
    // SELECT MULTIPLE BOOKS (For "Borrow/Return" Menu)
    // --------------------------------------------------------
    int selectBooks(Book* out[], int maxOut, const char* actionWord) {
        cout << "\nEnter ISBN / Title / Author: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        string query;
        getline(cin, query);

        query = toLowerString(query);

        // Try ISBN direct
        Book* b = catalog.search(query);
        if (b) {
            out[0] = b;
            return 1;
        }

        auto list = catalog.getAllBooks();
        Book* matches[50];
        int matchCount = 0;

        // 1. Find Partial Matches
        for (auto x : list) {
            if (containsIgnoreCase(x->title, query) || containsIgnoreCase(x->author, query)) {
                matches[matchCount++] = x;
                if (matchCount >= 50) break;
            }
        }

        // 2. If None, Find Suggestions (Fuzzy)
        bool isSuggestion = false;
        if (matchCount == 0) {
            for (auto x : list) {
                if (!query.empty() && !x->title.empty() &&
                    tolower((unsigned char)x->title[0]) == tolower((unsigned char)query[0])) {
                    matches[matchCount++] = x;
                    if (matchCount >= 5) break; // Limit suggestions
                }
            }
            if (matchCount > 0) isSuggestion = true;
        }

        // 3. If Empty
        if (matchCount == 0) {
            cout << "\nNo books found.\n";
            return 0;
        }

        // 4. Display Options
        if (isSuggestion) 
            cout << "\nNo exact match found. Did you mean one of these?\n";
        else 
            cout << "\nSelect book(s) to " << actionWord << ":\n";

        for (int i = 0; i < matchCount; i++) {
            cout << i + 1 << ". " << matches[i]->title
                 << " (ISBN: " << matches[i]->isbn << ") by "
                 << matches[i]->author << "\n";
        }

        // 5. Unified Selection Logic (Works for matches AND suggestions)
        cout << "\nEnter number(s) separated by space (example: 1 3) or 0 to cancel: ";

        int count = 0;
        while (true) {
            int x;
            if (!(cin >> x)) { // Catch invalid input (like letters)
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            if (x == 0) break;
            if (x >= 1 && x <= matchCount) {
                out[count++] = matches[x - 1];
                if (count == maxOut) break;
            }

            // Stop if next char is newline
            if (cin.peek() == '\n') break;
        }

        return count;
    }
};

#endif