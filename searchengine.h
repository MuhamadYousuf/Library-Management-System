#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
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

    // returns best exact/partial match, but also prints suggestions if not found
    Book* findBookInteractive() {
        cout << "\nEnter ISBN / Title / Author: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // flush leftover newline
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

        for (auto b : list) {
            if (containsIgnoreCase(b->title, query) || containsIgnoreCase(b->author, query)) {
                matches[matchCount++] = b;
                if (matchCount >= 50) break;
            }
        }

        if (matchCount == 0) {
            cout << "\nNo matching books found.\n\nDid you mean:\n";
            // show top 3 "close-ish" suggestions (simple: same first letter)
            int shown = 0;
            for (auto b : list) {
                if (!query.empty() && !b->title.empty() &&
                    tolower((unsigned char)b->title[0]) == tolower((unsigned char)query[0])) {
                    cout << "- " << b->title << " (ISBN: " << b->isbn << ") by " << b->author << "\n";
                    shown++;
                    if (shown == 3) break;
                }
            }
            if (shown == 0) cout << "- (no suggestions)\n";
            cout << "\n";
            return nullptr;
        }

        // if only 1 match, return it directly
        if (matchCount == 1) return matches[0];

        cout << "\nMultiple matches found:\n";
        for (int i = 0; i < matchCount; i++) {
            cout << i + 1 << ". " << matches[i]->title
                 << " (ISBN: " << matches[i]->isbn << ") by "
                 << matches[i]->author << "\n";
        }

        cout << "Choose 1-" << matchCount << " (0 to cancel): ";
        int choice;
        cin >> choice;

        if (choice < 1 || choice > matchCount) return nullptr;
        return matches[choice - 1];
    }

    // ✅ UPDATED: now prints "borrow" or "return" based on param
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

        for (auto x : list) {
            if (containsIgnoreCase(x->title, query) || containsIgnoreCase(x->author, query)) {
                matches[matchCount++] = x;
                if (matchCount >= 50) break;
            }
        }

        if (matchCount == 0) {
            cout << "\nBook not found.\n\nDid you mean:\n";
            int shown = 0;
            for (auto x : list) {
                if (!query.empty() && !x->title.empty() &&
                    tolower((unsigned char)x->title[0]) == tolower((unsigned char)query[0])) {
                    cout << "- " << x->title << " (ISBN: " << x->isbn << ") by " << x->author << "\n";
                    shown++;
                    if (shown == 3) break;
                }
            }
            if (shown == 0) cout << "- (no suggestions)\n";
            cout << "\n";
            return 0;
        }

        cout << "\nSelect book(s) to " << actionWord << ":\n";
        for (int i = 0; i < matchCount; i++) {
            cout << i + 1 << ". " << matches[i]->title
                 << " (ISBN: " << matches[i]->isbn << ") by "
                 << matches[i]->author << "\n";
        }

        cout << "\nEnter number(s) separated by space (example: 1 3 4) or 0 to cancel: ";

        int count = 0;
        while (true) {
            int x;
            cin >> x;

            if (x == 0) break;
            if (x >= 1 && x <= matchCount) {
                out[count++] = matches[x - 1];
                if (count == maxOut) break;
            }

            // stop if next char is newline
            if (cin.peek() == '\n') break;
        }

        return count;
    }
};

#endif
