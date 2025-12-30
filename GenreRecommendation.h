#ifndef GENRERECOMMENDATION_H
#define GENRERECOMMENDATION_H

#include <string>
#include <iostream>
#include "linkedlist.h"
#include "hashtable.h"
using namespace std;

class GenreRecommendation {
private:
    static const int GENRE_BUCKETS = 50;

    string genreNames[GENRE_BUCKETS];
    LinkedList<string> genreBooks[GENRE_BUCKETS];

    int genreCount;

    HashTable& catalog;

public:
    GenreRecommendation(HashTable& c) : catalog(c), genreCount(0) {}

    // returns index for genre OR creates new one
    int getGenreIndex(string genre) {
        for (int i = 0; i < genreCount; i++)
            if (genreNames[i] == genre)
                return i;

        // new genre
        if (genreCount < GENRE_BUCKETS) {
            genreNames[genreCount] = genre;
            return genreCount++;
        }

        return -1;  // no space (shouldn't happen)
    }

    // add ISBN under genre
    void addBook(string genre, string isbn) {
        int index = getGenreIndex(genre);
        if (index == -1) return;

        genreBooks[index].insert(isbn);
    }

    // recommend books from this genre
    void recommend(string genre) {
        int index = getGenreIndex(genre);
        if (index == -1) {
            cout << "\nUnknown genre.\n";
            return;
        }

        cout << "\nBooks in genre: " << genre << "\n";

        Node<string>* curr = genreBooks[index].getHead();
        if (!curr) {
            cout << "(No books yet in this genre)\n";
            return;
        }

        while (curr) {
            Book* b = catalog.search(curr->data);
            if (b && b->availableCopies > 0)   // <<< only recommend available books
                cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                curr = curr->next;
            }
    }
};

#endif
