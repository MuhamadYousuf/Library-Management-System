#ifndef BOOK_H
#define BOOK_H

#include <string>
#include "queue.h"     // IMPORTANT: include your custom queue

using namespace std;

struct Book {
    string isbn;
    string title;
    string author;
    string genre;
    int totalCopies;
    int availableCopies;

    Queue waitlist;        // custom queue
    int popularityCount;

    Book() {
        isbn = "";
        title = "";
        author = "";
        genre = "";
        totalCopies = 0;
        availableCopies = 0;
        popularityCount = 0;
    }

    Book(string i, string t, string a, int c, string g)
        : isbn(i), title(t), author(a), genre(g),
          totalCopies(c), availableCopies(c),
          popularityCount(0) {}
};

#endif
