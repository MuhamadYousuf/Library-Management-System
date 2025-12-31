#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <iostream>
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

    bool contains(const T& value) const {
        Node<T>* curr = head;
        while (curr) {
            if (curr->data == value) return true;
            curr = curr->next;
        }
        return false;
    }

    // removes first occurrence; returns true if removed
    bool removeOne(const T& value) {
        Node<T>* curr = head;
        Node<T>* prev = nullptr;

        while (curr) {
            if (curr->data == value) {
                if (prev == nullptr) head = curr->next;
                else prev->next = curr->next;
                delete curr;
                return true;
            }
            prev = curr;
            curr = curr->next;
        }
        return false;
    }

    Node<T>* getHead() const {
        return head;
    }

    void print() const {
        Node<T>* curr = head;
        while (curr) {
            cout << curr->data << " -> ";
            curr = curr->next;
        }
        cout << "NULL\n";
    }
};

#endif
