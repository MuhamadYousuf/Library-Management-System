# 📚 Smart Library Management & Recommendation System (C++ Project)

This project implements a fully functional *library management system* built entirely in C++ using **custom data structures**, without relying on STL containers (except queue).  
It supports book cataloging, borrowing/returning, waitlists, recommendations, popularity ranking, and full file storage.

This project was created as part of the **Data Structures (Fall 2025)** course.

---

# 🚀 Features

## 👤 User Features
- Search for books (ISBN, title, author)
- Borrow books
- Return books
- View borrowing history
- Receive book recommendations
- Automatic waitlist enrollment when copies are unavailable

---

## 🛠️ Admin Features
- Add/update/remove books from catalog
- Manage total/available copies
- View most borrowed books (popularity BST)
- View system reports:
  - Most popular books  
  - Overdue books  
  - Total users  
  - Total borrow operations  

---

# 🧠 Core Components

## 1️⃣ Catalog System
Uses a **Custom Hash Table** (Open Addressing + Linear Probing + Resizing) for:
- Fast ISBN lookup
- Insert/search/update/remove operations

Average time complexity: **O(1)**

---

## 2️⃣ Borrowing Engine
Implements:
- Borrow logic  
- Return logic  
- Waitlist system (queue per book)  
- Popularity tracking  
- History recording  

---

## 3️⃣ User Management System
Each user has:
- Unique ID  
- Name  
- Linked list borrowing history  
- Borrow count  
- Waitlist status  

---

## 4️⃣ Recommendation Engine (Graph-Based)
Books are represented as a graph:
Nodes → Books
Edges → Co-borrowing relationships


If users often borrow Book A and Book B together, an edge is formed.

Recommendations use:
- **BFS** for near-neighbor suggestions  
- **DFS** for deeper exploration  

---

## 5️⃣ Popularity Ranking (Balanced BST)
Books are inserted into a BST based on popularity count:
Left → less popular
Right → more popular


Supports:
- Top-K most borrowed books  
- Popularity reports  

---

## 6️⃣ File I/O (Persistent Storage)
All data is stored in `.txt` files:
- books.txt  
- users.txt  
- history_userID.txt  
- waitlists.txt  

System loads all files on startup & saves on exit.

---

# 🏗️ Project Architecture

SmartLibraryProject/
│
├── src/
│ ├── main.cpp
│ │
│ ├── models/
│ │ ├── book.h
│ │ ├── user.h
│ │ └── linkedlist.h
│ │
│ ├── data_structures/
│ │ ├── hashtable.h
│ │ ├── bst.h
│ │ ├── graph.h
│ │ └── queue.h (optional)
│ │
│ ├── core/
│ │ ├── usermanager.h
│ │ ├── borrowengine.h
│ │ ├── recommendationengine.h
│ │ └── reportengine.h
│ │
│ └── fileio/
│ ├── filemanager.h
│ └── parsers.h
│
├── data/
│ ├── books.txt
│ ├── users.txt
│ ├── history_*.txt
│ └── waitlists.txt
│
├── tests/
│ ├── hashtable_tests.cpp
│ ├── linkedlist_tests.cpp
│ └── bst_tests.cpp
│
├── docs/
│ ├── Week10_Presentation.pdf
│ ├── Week11_Documentation.pdf
│ ├── UML.png
│ └── file_formats.md
│
└── README.md

---

# 📦 Data Structures

| Feature | Data Structure | Reason |
|--------|----------------|--------|
| ISBN Search | Hash Table | O(1) lookup |
| Borrow History | LinkedList<T> | Insert-first, ordered log |
| Waitlist | Queue<int> | FIFO fairness |
| Recommendation | Graph (Adjacency List) | BFS/DFS relationships |
| Popularity Ranking | Balanced BST | Sorted popularity |
| User Storage | Array<User> | Fast indexed access |

---

# 🖥️ CLI (Command-Line Interface)

Users interact with the system through a structured menu:
===== SMART LIBRARY SYSTEM =====

1.Add Book

2.Search Book

3.Add User

4.Borrow Book

5.Return Book

6.View User History

7.View Recommendations

8.Most Popular Books

9.Exit



---

# 🧪 Unit Tests
Includes tests for:
- HashTable insert/search/resize  
- LinkedList insert/iterate  
- BorrowEngine borrow/return logic  
- BST insertion & traversal  

Stored in `/tests/`.

---

# ✅ Completed Milestones
### Week 9  
- Domain study  
- Initial DS mapping  
- Project plan  

### Week 10  
- Architecture  
- Presentation  
- Prototype demo  

### Week 11  
- Full foundation implementation (HashTable, LinkedList, User, Book, BorrowEngine)  
- CLI working  
- Documentation delivered  

### Week 12  
- File I/O  
- Recommendation graph  
- BST popularity  

### Week 13  
- Testing  
- Final report  
- Viva preparation  

---

# 👤 Author  
**Aiman**  
Data Structures — Fall 2025  
Advisor: *[Instructor Name]*  

---

# ⭐ Final Notes  
This project intentionally avoids STL containers like `vector`, `map`, and `unordered_map` to build a deeper understanding of **manual data structure implementation**, hashing, collision resolution, file handling, and pointer-based logic.

