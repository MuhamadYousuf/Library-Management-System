#ifndef USER_H
#define USER_H

#include "linkedlist.h"
using namespace std;

struct User {
    int userId;
    string name;
    LinkedList<string> history;

    User() {
        userId = -1;
        name = "";
    }

    User(int id, string n) {
        userId = id;
        name = n;
    }
};

#endif
