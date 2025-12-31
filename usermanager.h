#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "user.h"

class UserManager {
public:
    User userList[1000];
    int userCount;

    UserManager() : userCount(0) {}

    bool addUser(int id, string name) {
        if (getUser(id) != nullptr) return false; // prevent duplicates
        userList[userCount] = User(id, name);
        userCount++;
        return true;
    }

    User* getUser(int id) {
        for (int i = 0; i < userCount; i++)
            if (userList[i].userId == id)
                return &userList[i];
        return nullptr;
    }

    void addHistory(int id, string isbn) {
        User* u = getUser(id);
        if (u) u->history.insert(isbn);
    }
};

#endif
