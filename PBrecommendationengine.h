#ifndef PBRECOMMENDATIONENGINE_H
#define PBRECOMMENDATIONENGINE_H

#include "hashtable.h"
#include "maxheap.h"
#include <vector>

class PBRecommendationEngine {
private:
    HashTable& catalog;

public:
    PBRecommendationEngine(HashTable& c) : catalog(c) {}

    std::vector<Book*> getPopular(int k) {
        std::vector<Book*> allBooks = catalog.getAllBooks();
        MaxHeap heap;

        // Insert all books into heap
        for (Book* b : allBooks) {
            if (b->availableCopies > 0)   // <<< only available books
            heap.insert(b);
        }


        std::vector<Book*> result;

        // Extract top-k books
        for (int i = 0; i < k && !heap.isEmpty(); i++) {
            result.push_back(heap.extractMax());
        }

        return result;
    }
};

#endif
