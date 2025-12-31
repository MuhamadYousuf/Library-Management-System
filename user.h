#ifndef USER_H
#define USER_H

#include <string>
#include "linkedlist.h"
using namespace std;

struct User {
    int userId;
    string name;
    LinkedList<string> history;   
    LinkedList<string> borrowed;  
    User() : userId(-1), name("") {}
    User(int id, string n) : userId(id), name(n) {}
};

#endif
