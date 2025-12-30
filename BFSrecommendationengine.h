#ifndef BFSRECOMMENDATIONENGINE_H
#define BFSRECOMMENDATIONENGINE_H

#include <iostream>
#include <string>
#include "hashtable.h"

using namespace std;

class BFSRecommendation {
private:
    HashTable& catalog;

    // Graph Storage (Adjacency List)
    static const int MAX_BOOKS = 5000;

    string isbnList[MAX_BOOKS];          
    string neighbors[MAX_BOOKS][10];     
    int neighborCount[MAX_BOOKS];        
    int totalBooks;

public:
    BFSRecommendation(HashTable& c) : catalog(c), totalBooks(0) {
        for (int i = 0; i < MAX_BOOKS; i++)
            neighborCount[i] = 0;
    }

    // ----------- Register ISBN in graph -----------
    int getIndex(string isbn) {
        for (int i = 0; i < totalBooks; i++)
            if (isbnList[i] == isbn) return i;

        isbnList[totalBooks] = isbn;
        neighborCount[totalBooks] = 0;
        return totalBooks++;
    }

    // ----------- Add Edge (Co-borrow Relationship) -----------
    void addEdge(string a, string b) {
        int i = getIndex(a);
        int j = getIndex(b);

        if (neighborCount[i] < 10)
            neighbors[i][neighborCount[i]++] = b;

        if (neighborCount[j] < 10)
            neighbors[j][neighborCount[j]++] = a;
    }

    // ----------- BFS Traversal -----------
    void traverse(string startIsbn) {
        cout << "\nBFS Recommendation Order:\n";

        bool visited[MAX_BOOKS] = {false};
        string queueArr[MAX_BOOKS];
        int front = 0, back = 0;

        int startIndex = getIndex(startIsbn);

        // Push starting ISBN
        queueArr[back++] = startIsbn;
        visited[startIndex] = true;

        while (front < back) {
            string currIsbn = queueArr[front++];

            Book* b = catalog.search(currIsbn);
            if (b && b->availableCopies > 0) {     // <<< only books available
                cout << b->title 
                << " (Popularity: " << b->popularityCount << ")\n";
            }


            int currIdx = getIndex(currIsbn);

            // Visit neighbors
            for (int i = 0; i < neighborCount[currIdx]; i++) {
                string nextIsbn = neighbors[currIdx][i];
                int nextIdx = getIndex(nextIsbn);

                if (!visited[nextIdx]) {
                    visited[nextIdx] = true;
                    queueArr[back++] = nextIsbn;
                }
            }
        }
    }
};

#endif
