#ifndef USER_H
#define USER_H

#include "linkedlist.h"
using namespace std;

struct User {
    int userId;
    string name;
    int borrowedCount;
    LinkedList<string> history;

    User() {
        userId = -1;
        name = "";
        borrowedCount = 0;
    }

    User(int id, string n) : userId(id), name(n), borrowedCount(0) {}
};

#endif
