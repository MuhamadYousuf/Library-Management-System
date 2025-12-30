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
    string genre;


    queue<int> waitlist;
    int popularityCount;

    Book() {
        isbn = "";
        title = "";
        author = "";
        totalCopies = 0;
        availableCopies = 0;
        popularityCount = 0;
    }

    Book(string i, string t, string a, int c, string g)
    : isbn(i), title(t), author(a), totalCopies(c), availableCopies(c), popularityCount(0), genre(g) {}

};

#endif
