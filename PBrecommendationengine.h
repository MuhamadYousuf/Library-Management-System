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