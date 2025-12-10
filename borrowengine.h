#ifndef BORROWENGINE_H
#define BORROWENGINE_H

#include "hashtable.h"
#include "usermanager.h"

class BorrowEngine {
private:
    HashTable& catalog;
    UserManager& users;

public:
    BorrowEngine(HashTable& c, UserManager& u)
        : catalog(c), users(u) {}

    bool borrowBook(int userId, string isbn) {
        Book* b = catalog.search(isbn);
        if (!b) return false;

        if (b->availableCopies > 0) {
            b->availableCopies--;
            users.addHistory(userId, isbn);
            b->popularityCount++;
            return true;
        }

        b->waitlist.push(userId);
        return false;
    }

    bool returnBook(int userId, string isbn) {
        Book* b = catalog.search(isbn);
        if (!b) return false;

        if (!b->waitlist.empty()) {
            int nextUser = b->waitlist.front();
            b->waitlist.pop();
            users.addHistory(nextUser, isbn);
        } else {
            b->availableCopies++;
        }
        return true;
    }
};

#endif
