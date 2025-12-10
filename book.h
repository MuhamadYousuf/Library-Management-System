#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <queue>
using namespace std;

struct Book {
    string isbn;
    string title;
    string author;
    int totalCopies;
    int availableCopies;

    queue<int> waitlist;    // user IDs
    int popularityCount;

    Book() {
        isbn = "";
        title = "";
        author = "";
        totalCopies = 0;
        availableCopies = 0;
        popularityCount = 0;
    }

    Book(string i, string t, string a, int tot)
        : isbn(i), title(t), author(a), totalCopies(tot),
          availableCopies(tot), popularityCount(0) {}
};

#endif
