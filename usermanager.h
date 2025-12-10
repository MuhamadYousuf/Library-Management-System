#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "user.h"

class UserManager {
private:
    User users[1000];   // array instead of vector
    int userCount;

public:
    UserManager() : userCount(0) {}

    void addUser(int id, string name) {
        users[userCount] = User(id, name);
        userCount++;
    }

    User* getUser(int id) {
        for (int i = 0; i < userCount; i++)
            if (users[i].userId == id)
                return &users[i];
        return nullptr;
    }

    void addHistory(int id, string isbn) {
        User* u = getUser(id);
        if (u)
            u->history.insert(isbn);
    }
};

#endif
