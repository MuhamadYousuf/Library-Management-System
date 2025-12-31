#ifndef QUEUE_H
#define QUEUE_H

struct QNode {
    int data;
    QNode* next;
    QNode(int d) : data(d), next(nullptr) {}
};

class Queue {
private:
    QNode* frontPtr;
    QNode* rearPtr;

public:
    Queue() : frontPtr(nullptr), rearPtr(nullptr) {}

    bool isEmpty() const {
        return frontPtr == nullptr;
    }

    void enqueue(int x) {
        QNode* n = new QNode(x);
        if (rearPtr == nullptr) {
            frontPtr = rearPtr = n;
            return;
        }
        rearPtr->next = n;
        rearPtr = n;
    }

    // returns false if empty
    bool dequeue(int& out) {
        if (isEmpty()) return false;

        QNode* temp = frontPtr;
        out = temp->data;

        frontPtr = frontPtr->next;
        if (frontPtr == nullptr) rearPtr = nullptr;

        delete temp;
        return true;
    }

    // used for file saving (traversal)
    QNode* getFront() const {
        return frontPtr;
    }

    // prevent duplicate userId in same waitlist
    bool contains(int x) const {
        QNode* curr = frontPtr;
        while (curr) {
            if (curr->data == x) return true;
            curr = curr->next;
        }
        return false;
    }
};

#endif
