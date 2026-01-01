#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "book.h"
#include <iostream>
#include <vector>

using namespace std;

class HashTable {
private:
    int capacity;
    int size;
    Book* table;

    // Hash function
    int hashFunc(string key) {
        unsigned long hash = 0;
        for (char c : key)
            hash = hash * 31 + c;
        return hash % capacity;
    }

    // Resize table when load factor > 0.7
    void resize() {
        int oldCapacity = capacity;
        Book* oldTable = table;

        capacity *= 2;
        table = new Book[capacity];
        size = 0;  // we'll reinsert everything

        for (int i = 0; i < oldCapacity; i++) {
            if (oldTable[i].isbn != "") {
                insert(oldTable[i]);   // reinsert actual Book as-is
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

    // Insert book using linear probing
    void insert(Book b) {
        // 1. Check if ISBN already exists
        Book* existing = search(b.isbn);
        if (existing != nullptr) {
            // Update the existing book instead
            existing->title = b.title;
            existing->author = b.author;
            existing->genre = b.genre;
            existing->totalCopies = b.totalCopies;
            existing->availableCopies = b.availableCopies;
            return;
        }

        // 2. Resize if needed
        if ((float)size / capacity > 0.7)
            resize();

        // 3. Insert normally
        int index = hashFunc(b.isbn);

        while (table[index].isbn != "") {
            index = (index + 1) % capacity;
        }

        table[index] = b;
        size++;
    }

    // ✅ NEW FEATURE: Remove book
    bool remove(string isbn) {
        int index = hashFunc(isbn);
        int start = index;

        while (table[index].isbn != "") {
            if (table[index].isbn == isbn) {
                // Found it. Clear the slot.
                table[index].isbn = ""; 
                size--;

                // Rehash the chain: subsequent items might belong to this index
                index = (index + 1) % capacity;
                while (table[index].isbn != "") {
                    Book temp = table[index];     
                    table[index].isbn = "";       // clear old spot
                    size--;                       // size dec (insert will inc it back)
                    insert(temp);                 // re-insert to find correct spot
                    index = (index + 1) % capacity;
                }
                return true;
            }
            index = (index + 1) % capacity;
            if (index == start) break;
        }
        return false;
    }

    // Search for book
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

    vector<Book*> getAllBooks() {
        vector<Book*> books;
        books.reserve(size);

        for (int i = 0; i < capacity; i++) {
            if (table[i].isbn != "")
                books.push_back(&table[i]);
        }
        return books;
    }
};

#endif