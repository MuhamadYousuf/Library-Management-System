#ifndef GENRERECOMMENDATION_H
#define GENRERECOMMENDATION_H

#include <string>
#include <iostream>
#include "hashtable.h"

class GenreRecommendation {
private:
    static const int MAX_GENRES = 100;
    static const int MAX_BOOKS = 5000;

    string genres[MAX_GENRES];
    string booksInGenre[MAX_GENRES][MAX_BOOKS];
    int bookCount[MAX_GENRES];
    int genreCount;

    HashTable& catalog;

public:
    GenreRecommendation(HashTable& c) : catalog(c), genreCount(0) {
        for (int i = 0; i < MAX_GENRES; i++)
            bookCount[i] = 0;
    }

    // Find genre index or create a new one
    int getGenreIndex(string genre) {
        for (int i = 0; i < genreCount; i++)
            if (genres[i] == genre) return i;

        genres[genreCount] = genre;
        bookCount[genreCount] = 0;
        return genreCount++;
    }

    // Add ISBN to genre
    void addBook(string genre, string isbn) {
        int index = getGenreIndex(genre);

        if (bookCount[index] < MAX_BOOKS)
            booksInGenre[index][bookCount[index]++] = isbn;
    }

    // Recommend similar-genre books
    void recommend(string genre) {
        int index = getGenreIndex(genre);

        cout << "\nBooks in Genre: " << genre << "\n";

        for (int i = 0; i < bookCount[index]; i++) {
            Book* b = catalog.search(booksInGenre[index][i]);
            if (b)
                cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
        }
    }
};

#endif
