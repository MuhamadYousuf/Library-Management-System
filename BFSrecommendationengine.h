#ifndef BFSRECOMMENDATIONENGINE_H
#define BFSRECOMMENDATIONENGINE_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include "hashtable.h"
#include "usermanager.h"

using namespace std;

class BFSRecommendation {
private:
    HashTable& catalog;

    // Use Vectors to store data on the Heap (prevents Stack Overflow)
    vector<string> isbnList;
    vector<vector<string>> adj; // Adjacency list

public:
    BFSRecommendation(HashTable& c) : catalog(c) {}

    // ----------- Register ISBN in graph -----------
    int getIndex(string isbn) {
        // Linear search (simple for this size)
        for (int i = 0; i < (int)isbnList.size(); i++) {
            if (isbnList[i] == isbn) return i;
        }

        // Add new ISBN dynamically
        isbnList.push_back(isbn);
        adj.push_back(vector<string>()); // Create empty neighbor list
        return (int)isbnList.size() - 1;
    }

    // ----------- Add Edge (Co-borrow Relationship) -----------
    void addEdge(string a, string b) {
        if (a == b) return; 

        int i = getIndex(a);
        int j = getIndex(b);

        // Check if connection already exists in i's list
        bool found = false;
        for (const string& neighbor : adj[i]) {
            if (neighbor == b) { found = true; break; }
        }
        
        if (!found) {
            adj[i].push_back(b);
        }

        // Add reverse connection in j's list
        found = false;
        for (const string& neighbor : adj[j]) {
            if (neighbor == a) { found = true; break; }
        }

        if (!found) {
            adj[j].push_back(a);
        }
    }

    // ----------- Build graph from actual history -----------
    void buildGraph(UserManager& users) {
        // Clear previous data if needed
        isbnList.clear();
        adj.clear();

        for (int i = 0; i < users.userCount; i++) {
            User& u = users.userList[i];
            
            // Loop through user's history to find pairs
            Node<string>* outer = u.history.getHead();
            while (outer) {
                Node<string>* inner = outer->next;
                while (inner) {
                    addEdge(outer->data, inner->data);
                    inner = inner->next;
                }
                outer = outer->next;
            }
        }
    }

    // ----------- BFS Traversal -----------
    void traverse(string startIsbn) {
        cout << "\nBFS Recommendation (Based on what others also borrowed):\n";

        int startIdx = getIndex(startIsbn);
        if (startIdx == -1) {
            cout << "(No sufficient data to make recommendations for this book)\n";
            return;
        }

        // Use vector for visited to match dynamic size
        vector<bool> visited(isbnList.size(), false);
        queue<string> q;

        // Initialize BFS
        visited[startIdx] = true;
        q.push(startIsbn);

        bool foundAny = false;

        while (!q.empty()) {
            string currIsbn = q.front();
            q.pop();

            // Recommend this book (skip the starting one)
            if (currIsbn != startIsbn) {
                Book* b = catalog.search(currIsbn);
                if (b && b->availableCopies > 0) {
                    cout << "- " << b->title 
                         << " (Popularity: " << b->popularityCount << ")\n";
                    foundAny = true;
                }
            }

            int currIdx = getIndex(currIsbn);
            if (currIdx == -1) continue;

            // Visit neighbors
            for (const string& neighbor : adj[currIdx]) {
                int nextIdx = getIndex(neighbor);
                if (nextIdx != -1 && !visited[nextIdx]) {
                    visited[nextIdx] = true;
                    q.push(neighbor);
                }
            }
        }

        if (!foundAny) cout << "(No similar available books found)\n";
    }
};

#endif