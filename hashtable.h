#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "book.h"
#include <iostream>

class HashTable {
private:
    int capacity;
    int size;
    Book* table;

    int hashFunc(string key) {
        unsigned long hash = 0;
        for (char c : key)
            hash = hash * 31 + c;
        return hash % capacity;
    }

    void resize() {
        int oldCapacity = capacity;
        capacity *= 2;

        Book* oldTable = table;
        table = new Book[capacity];

        for (int i = 0; i < oldCapacity; i++) {
            if (oldTable[i].isbn != "") {
                insert(oldTable[i]);
            }
        }

        delete[] oldTable;
    }

public:
    HashTable(int cap = 20) {
        capacity = cap;
        size = 0;
        table = new Book[capacity];
    }

    void insert(Book b) {
        if ((float)size / capacity > 0.7)
            resize();

        int index = hashFunc(b.isbn);

        while (table[index].isbn != "") {
            index = (index + 1) % capacity;
        }

        table[index] = b;
        size++;
    }

    Book* search(string isbn) {
        int index = hashFunc(isbn);
        int start = index;

        while (table[index].isbn != "") {
            if (table[index].isbn == isbn)
                return &table[index];
            index = (index + 1) % capacity;

            if (index == start)
                break;
        }
        return nullptr;
    }
};

#endif
