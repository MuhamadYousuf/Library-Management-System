#ifndef BORROWENGINE_H
#define BORROWENGINE_H

#include "hashtable.h"
#include "usermanager.h"

class BorrowEngine {
private:
    HashTable& catalog;
    UserManager& users;

public:
    BorrowEngine(HashTable& c, UserManager& u) : catalog(c), users(u) {}

    // return true if borrowed, false otherwise
    bool borrowBook(int userId, string isbn) {
        Book* b = catalog.search(isbn);
        if (!b) return false;

        User* u = users.getUser(userId);
        if (!u) return false;

        // 1 user cannot borrow same book twice
        if (u->borrowed.contains(isbn)) {
            return false;
        }

        // copies available
        if (b->availableCopies > 0) {
            b->availableCopies--;
            b->popularityCount++;

            u->borrowed.insert(isbn);
            users.addHistory(userId, isbn);
            return true;
        }

        // no copies -> waitlist, but no duplicates
        if (!b->waitlist.contains(userId)) {
            b->waitlist.enqueue(userId);
        }
        return false;
    }

    // return true if successful return, false otherwise
    bool returnBook(int userId, string isbn) {
        Book* b = catalog.search(isbn);
        if (!b) return false;

        User* u = users.getUser(userId);
        if (!u) return false;

        // user must have borrowed it
        if (!u->borrowed.removeOne(isbn)) {
            return false;
        }

        // if waitlist has someone, give book to next user
        int nextUser;
        if (b->waitlist.dequeue(nextUser)) {
            User* nu = users.getUser(nextUser);
            if (nu) {
                nu->borrowed.insert(isbn);
                users.addHistory(nextUser, isbn);
            } else {
                // if user doesn't exist, give back to inventory
                b->availableCopies++;
            }
        } else {
            b->availableCopies++;
        }

        return true;
    }
};

#endif
