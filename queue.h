#ifndef QUEUE_H
#define QUEUE_H

// A simple linked-list-based queue for integers (User IDs)

struct QNode {
    int data;
    QNode* next;

    QNode(int d) : data(d), next(nullptr) {}
};

class Queue {
private:
    QNode* front;
    QNode* rear;

public:
    Queue() : front(nullptr), rear(nullptr) {}

    bool isEmpty() {
        return front == nullptr;
    }

    void push(int x) {
        QNode* n = new QNode(x);

        if (rear == nullptr) {
            front = rear = n;
            return;
        }

        rear->next = n;
        rear = n;
    }

    void pop() {
        if (isEmpty()) return;

        QNode* temp = front;
        front = front->next;
        delete temp;

        if (front == nullptr)
            rear = nullptr;
    }

    int peek() {
        if (isEmpty()) return -1;  // or throw exception
        return front->data;
    }

    QNode* getFront() {
        return front;
    }

    // Clear queue
    void clear() {
        while (!isEmpty()) pop();
    }
};

#endif
