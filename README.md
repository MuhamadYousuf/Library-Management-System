# 📚 Smart Library Management & Recommendation System

> A high-performance, console-based library system built in C++ using **custom Data Structures** (Hash Tables, Graphs, Heaps, and Linked Lists) to handle inventory, users, and intelligent recommendations.

---

## 🚀 Overview

This project is a fully functional Library Management System designed to demonstrate the practical application of advanced Data Structures and Algorithms. Unlike typical CRUD apps, this system implements core structures from scratch to optimize performance for specific tasks—like **$O(1)$** book lookups and **Graph-based** collaborative filtering.

It features separate portals for **Admins** (inventory management) and **Users** (borrowing/returning), with persistent data storage using flat files.

---

## ✨ Key Features

### 🧠 Intelligent Recommendations
* **Collaborative Filtering (BFS):** Suggests books based on reading patterns using a "Co-Borrowing Graph" (e.g., "Users who read *Harry Potter* also read *The Hobbit*").
* **Popularity Engine (Max Heap):** Instantly retrieves the top trending books using a Binary Max Heap.
* **Content-Based:** Recommendations by Genre buckets.

### ⚡ High-Performance Core
* **Custom Hash Table:** Manages the book catalog with **Linear Probing** for collision resolution, ensuring instant search results.
* **Smart Search Engine:** Features **Fuzzy Matching** and **Typo Tolerance** (e.g., searching "hary poter" finds "Harry Potter").
* **Waitlist System (Queue):** Automatically queues users when a book is out of stock and assigns it to the next person upon return.

### 📂 Data Persistence
* Automatically saves and loads all data (`books.txt`, `users.txt`, `history.txt`, `waitlist.txt`, `borrowed.txt`) to ensure no data is lost between sessions.

---

## 🛠️ Data Structures Used

| Component | Data Structure | Why? |
| :--- | :--- | :--- |
| **Book Catalog** | `HashTable` | For **$O(1)$** average time complexity during ISBN lookups. |
| **User History** | `LinkedList<T>` | To efficiently handle dynamic, growing lists of read books per user. |
| **Waitlists** | `Queue` | To enforce **FIFO (First-In-First-Out)** fairness for popular books. |
| **Trending Books**| `MaxHeap` | To extract the "Top K" most borrowed books in **$O(\log n)$** time. |
| **Recommendations**| `Graph` (Adj List)| To model relationships between books for Breadth-First Search (BFS). |

---

## ⚙️ Installation & Usage

### Prerequisites
* C++ Compiler (G++, Clang, or MSVC) supporting C++17 or later.

### Build Instructions
1.  **Clone the repository:**
    ```bash
    git clone [https://github.com/MuhamadYousuf/smart-library-system.git](https://github.com/MuhamadYousuf/smart-library-system.git)
    cd smart-library-system
    ```

2.  **Compile the code:**
    ```bash
    g++ main.cpp -o library_app
    ```

3.  **Run the application:**
    * **Windows:** `library_app.exe`
    * **Mac/Linux:** `./library_app`

---

## 📖 User Guide

### 1. Admin Portal
* **Default Credentials:** `admin` / `1234`
* **Capabilities:** Add books, register users, view user activity logs, remove books, and view popularity reports.

### 2. User Portal
* **Login:** Enter your unique **User ID**.
* **Capabilities:**
    * **Search:** Type Title, Author, or ISBN (handles partial matches).
    * **Borrow:** Rent up to 3 books. (Auto-waitlist if unavailable).
    * **Return:** Select from a numbered list of your currently held books.
    * **Get Recommendations:** Choose between Popularity, BFS (Similar Users), or Genre.

---

## 📂 Project Structure

```text
├── main.cpp                 # Entry point & Menu logic
├── books.txt                # Database file for Books
├── users.txt                # Database file for Users
├── history.txt              # Database file for Reading History
├── borrowed.txt             # Database file for Current Loans
├── waitlist.txt             # Database file for Queues
│
├── headers/
│   ├── book.h               # Book Data Model
│   ├── user.h               # User Data Model
│   ├── hashtable.h          # Custom Hash Table Implementation
│   ├── linkedlist.h         # Custom Linked List Template
│   ├── queue.h              # Custom Queue Implementation
│   ├── maxheap.h            # Binary Max Heap for Popularity
│   ├── usermanager.h        # User Authentication & Management
│   ├── borrowengine.h       # Transaction Logic (Borrow/Return)
│   ├── searchengine.h       # Fuzzy Search & Input Handling
│   ├── BFSrecommendationengine.h  # Graph-based Recommendation Engine
│   ├── PBrecommendationengine.h   # Popularity-based Engine
│   └── GenreRecommendation.h      # Genre-based Engine
