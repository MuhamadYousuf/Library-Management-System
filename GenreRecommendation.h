#ifndef GENRERECOMMENDATION_H
#define GENRERECOMMENDATION_H

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cctype> // for tolower
#include "linkedlist.h"
#include "hashtable.h"

using namespace std;

class GenreRecommendation {
private:
    static const int GENRE_BUCKETS = 50;

    string genreNames[GENRE_BUCKETS];        // The "Labels" (e.g., "SciFi")
    LinkedList<string> genreBooks[GENRE_BUCKETS]; // The "Buckets" (List of ISBNs)

    int genreCount;
    HashTable& catalog;

    // ---------------------------------------------------------
    // HELPER FUNCTIONS (Private)
    // ---------------------------------------------------------
    string toLowerString(string s) {
        for (int i = 0; i < (int)s.size(); i++)
            s[i] = (char)tolower((unsigned char)s[i]);
        return s;
    }

    bool containsIgnoreCase(const string& haystack, const string& needle) {
        string h = toLowerString(haystack);
        string n = toLowerString(needle);
        return (h.find(n) != string::npos);
    }

    // Finds a genre index without creating a new one (Search Mode)
    // Returns -1 if not found.
    int findGenreIndex(string query) {
        string q = toLowerString(query);

        // 1. Try Exact Match (Case Insensitive)
        for (int i = 0; i < genreCount; i++) {
            if (toLowerString(genreNames[i]) == q) return i;
        }

        // 2. Try Partial Match (e.g., "fanta" -> "Fantasy")
        for (int i = 0; i < genreCount; i++) {
            if (containsIgnoreCase(genreNames[i], query)) return i;
        }

        return -1;
    }

public:
    GenreRecommendation(HashTable& c) : catalog(c), genreCount(0) {}

    // ---------------------------------------------------------
    // ADD BOOK (Used during loading)
    // ---------------------------------------------------------
    // This finds the genre bucket. If it doesn't exist, it CREATES it.
    void addBook(string genre, string isbn) {
        // 1. Check if genre bucket already exists (Exact/Fuzzy)
        int index = findGenreIndex(genre);

        // 2. If not found, create a new bucket
        if (index == -1) {
            if (genreCount < GENRE_BUCKETS) {
                genreNames[genreCount] = genre; // Store the original casing (e.g. "SciFi")
                index = genreCount;
                genreCount++;
            } else {
                return; // No space left for new genres
            }
        }

        // 3. Add ISBN to the bucket
        genreBooks[index].insert(isbn);
    }

    // ---------------------------------------------------------
    // RECOMMEND (User Facing)
    // ---------------------------------------------------------
    void recommend(string query) {
        // 1. Use the robust search helper
        int index = findGenreIndex(query);

        if (index == -1) {
            cout << "\nUnknown genre. Try 'SciFi', 'Tech', 'Fantasy', etc.\n";
            return;
        }

        // 2. Print the results
        string foundName = genreNames[index];
        cout << "\n[Genre Match]: " << foundName << "\n";
        cout << "Here are the top books in " << foundName << ":\n";

        Node<string>* curr = genreBooks[index].getHead();
        if (!curr) {
            cout << "(No books yet in this genre)\n";
            return;
        }

        bool foundAny = false;
        while (curr) {
            Book* b = catalog.search(curr->data);
            // Only recommend if book exists AND is available
            if (b && b->availableCopies > 0) {
                cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                foundAny = true;
            }
            curr = curr->next;
        }

        if (!foundAny) {
            cout << "(All books in this genre are currently borrowed out)\n";
        }
    }
};

#endif