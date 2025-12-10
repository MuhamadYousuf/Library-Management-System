# Week 11 Progress Report  
Smart Library Management & Recommendation System — C++ Project  

## 1. Overview  
This week focused on completing the foundation of the system, including all core data structures, class definitions, and a working command-line interface (CLI).  
The system now supports users, books, borrowing, returning, and maintaining user borrowing history.

---

## 2. Work Completed This Week  

### ✔ Core Data Structures Implemented  
- **Custom Hash Table (Open Addressing + Linear Probing + Resizing)**  
  - Used for O(1) ISBN lookup  
  - Supports insert, search, automatic rehashing  

- **LinkedList<T>**  
  - Used for user borrowing history  

- **User and Book Models**  
  - Book includes waitlist queue + popularity counter  
  - User includes linked list history  

### ✔ Core Modules Implemented  
- **UserManager**  
  - Add users  
  - Fetch users  
  - Append to user’s history  

- **BorrowEngine**  
  - Borrow logic  
  - Return logic with waitlist handling  
  - Popularity update  

- **HashTable + UserManager Integration**  
  - Full borrow flow: search → check availability → update history → waitlist  

### ✔ Working CLI  
A fully interactive terminal menu was added with options:

1. Add Book  
2. Search Book  
3. Add User  
4. Borrow Book  
5. Return Book  
6. View User Borrowing History  
7. Exit  

CLI supports user-driven manual testing of the system.

---

## 3. Testing Performed  
- Insert/search operations in hash table tested with collision cases  
- User addition and lookup tested  
- Borrowing a book when copies are available  
- Borrowing when copies are NOT available (waitlist works)  
- Returning a book triggers next user in waitlist  
- Viewing user history now prints actual **book titles**, not raw ISBNs  

All foundation features are working correctly.

---

## 4. Challenges Faced  
- Understanding hash table collision handling and rehashing  
- Ensuring linear probing handles wrap-around correctly  
- Designing history printing to show full book details instead of ISBNs  
- Avoiding vectors to keep implementation consistent with course requirements  

---

## 5. Next Steps (Week 12 Goals)  
- Add **File I/O**: load/save books, users, history  
- Implement **co-borrowing graph** using adjacency lists  
- Implement **recommendation engine** (BFS/DFS)  
- Add **Balanced BST / Priority Queue** for popularity ranking  
- Expand CLI with reporting + admin tools  

---

## 6. Current Status  
**The project is fully on track.**  
All Week-11 deliverables have been completed ahead of schedule.  

System foundation is stable and ready for Week-12 implementation.
