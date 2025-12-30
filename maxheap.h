#ifndef MAXHEAP_H
#define MAXHEAP_H

#include "book.h"

class MaxHeap {
private:
    Book* arr[10000];
    int size;

public:
    MaxHeap() : size(0) {}

    bool isEmpty() const {
        return size == 0;
    }

    void insert(Book* b) {
        arr[size] = b;
        heapifyUp(size);
        size++;
    }

    Book* extractMax() {
        if (size == 0) return nullptr;

        Book* maxBook = arr[0];
        arr[0] = arr[size - 1];
        size--;
        heapifyDown(0);
        return maxBook;
    }

    int getSize() const {
        return size;
    }

private:
    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;

            if (arr[index]->popularityCount > arr[parent]->popularityCount) {
                Book* temp = arr[index];
                arr[index] = arr[parent];
                arr[parent] = temp;
                index = parent;
            }
            else break;
        }
    }

    void heapifyDown(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < size && arr[left]->popularityCount > arr[largest]->popularityCount)
                largest = left;

            if (right < size && arr[right]->popularityCount > arr[largest]->popularityCount)
                largest = right;

            if (largest != index) {
                Book* temp = arr[index];
                arr[index] = arr[largest];
                arr[largest] = temp;
                index = largest;
            }
            else break;
        }
    }
};

#endif
