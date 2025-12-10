# Smart Library Management & Recommendation System (C++ Project)

## Overview
A C++ console-based system that manages library operations using efficient custom data structures:
- Custom Hash Table (Open Addressing + Resizing)
- Linked Lists
- Queue-based waitlists
- Graph-based recommendation engine
- Balanced BST for popularity ranking

## Features
### User:
- Search books by ISBN
- Borrow books
- Return books
- View borrowing history
- Receive recommendations

### Admin:
- Add, update, remove books
- View library reports

## Data Structures
- ISBN Lookup: Custom Hash Table
- User History: LinkedList<T>
- Waitlist: queue<int>
- Recommendations: Graph (Adjacency List)
- Popularity Ranking: Balanced BST

## File Formats
See docs/file_formats.md for detailed structure.

## Week 11 Progress
- Implemented core classes: Book, User, HashTable, LinkedList, UserManager, BorrowEngine.
- Added CLI for basic interaction.
- Verified borrow/return functionality and waitlists.
- Next steps: File I/O, Recommendation graph, BST-based ranking.
