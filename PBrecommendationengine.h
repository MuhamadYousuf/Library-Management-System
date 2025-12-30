#ifndef PBRECOMMENDATIONENGINE_H
#define PBRECOMMENDATIONENGINE_H

#include "hashtable.h"
#include "maxheap.h"
#include <vector>

class RecommendationEngine {
private:
    HashTable& catalog;

public:
    RecommendationEngine(HashTable& c) : catalog(c) {}

    std::vector<Book*> getPopular(int k) {
        std::vector<Book*> allBooks = catalog.getAllBooks();
        MaxHeap heap;

        for (Book* b : allBooks) {
            heap.insert(b);
        }

        std::vector<Book*> result;

        for (int i = 0; i < k && !heap.isEmpty(); i++) {
            result.push_back(heap.extractMax());
        }

        return result;
    }
};

#endif