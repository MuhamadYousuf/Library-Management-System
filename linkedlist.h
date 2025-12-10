#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <string>
using namespace std;

template <typename T>
struct Node {
    T data;
    Node* next;
    Node(T d) : data(d), next(nullptr) {}
};

template <typename T>
class LinkedList {
private:
    Node<T>* head;

public:
    LinkedList() : head(nullptr) {}

    void insert(T value) {
        Node<T>* n = new Node<T>(value);
        n->next = head;
        head = n;
    }

    void print() {
        Node<T>* curr = head;
        while (curr) {
            cout << curr->data << " -> ";
            curr = curr->next;
        }
        cout << "NULL\n";
    }

    Node<T>* getHead() {
    return head;
    }
};

#endif
