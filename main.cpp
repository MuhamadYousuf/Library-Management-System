#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
using namespace std;

#include "hashtable.h"
#include "usermanager.h"
#include "borrowengine.h"
#include "PBrecommendationengine.h"
#include "BFSrecommendationengine.h"
#include "GenreRecommendation.h"

/* =========================================================
                 UTILITY: TRIM WHITESPACE
   ========================================================= */

string trim(const string& s) {
    int start = 0, end = s.size() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t'))
        start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t'))
        end--;
    return s.substr(start, end - start + 1);
}

/* =========================================================
                       SAFE FILE LOADERS
   ========================================================= */

void ensureFileExists(string filename) {
    ifstream f(filename);
    if (!f) {
        ofstream create(filename);
        create.close();
    }
}

bool safeStoi(const string& s, int& out) {
    try {
        if (s == "" || s == " " || s == "\t")
            return false;
        out = stoi(s);
        return true;
    } catch (...) {
        return false;
    }
}

void loadBooks(HashTable& catalog, GenreRecommendation& genreRec) {
    ensureFileExists("books.txt");
    ifstream file("books.txt");

    string line;
    while (getline(file, line)) {
        if (line == "" || line.find('|') == string::npos) continue;

        stringstream ss(line);
        string isbn, title, author, genre, total, available, pop;

        getline(ss, isbn, '|');
        getline(ss, title, '|');
        getline(ss, author, '|');
        getline(ss, genre, '|');
        getline(ss, total, '|');
        getline(ss, available, '|');
        getline(ss, pop, '|');

        isbn = trim(isbn);
        title = trim(title);
        author = trim(author);
        genre = trim(genre);

        int totalInt = 1, availInt = 1, popInt = 0;
        safeStoi(total, totalInt);
        safeStoi(available, availInt);
        safeStoi(pop, popInt);

        if (isbn == "") continue;

        Book b(isbn, title, author, totalInt, genre);
        b.availableCopies = availInt;
        b.popularityCount = popInt;

        catalog.insert(b);
        genreRec.addBook(genre, isbn);
    }
}

void loadUsers(UserManager& users) {
    ensureFileExists("users.txt");
    ifstream file("users.txt");

    string line;
    while (getline(file, line)) {
        if (line == "" || line.find('|') == string::npos) continue;

        stringstream ss(line);
        string idStr, name;

        getline(ss, idStr, '|');
        getline(ss, name, '|');

        int id;
        if (!safeStoi(idStr, id)) continue;

        name = trim(name);

        users.addUser(id, name);
    }
}

void loadHistory(UserManager& users, BFSRecommendation& bfsRec) {
    ensureFileExists("history.txt");
    ifstream file("history.txt");

    string line;
    while (getline(file, line)) {
        if (line == "" || line.find('|') == string::npos) continue;

        stringstream ss(line);
        string idStr, isbn;

        getline(ss, idStr, '|');
        int userId;
        if (!safeStoi(idStr, userId)) continue;

        User* u = users.getUser(userId);
        if (!u) continue;

        string prev = "";

        while (getline(ss, isbn, '|')) {
            isbn = trim(isbn);
            if (isbn == "") continue;

            u->history.insert(isbn);

            if (prev != "")
                bfsRec.addEdge(prev, isbn);

            prev = isbn;
        }
    }
}

void loadWaitlist(HashTable& catalog) {
    ensureFileExists("waitlist.txt");
    ifstream file("waitlist.txt");

    string line;
    while (getline(file, line)) {
        if (line == "" || line.find('|') == string::npos) continue;

        stringstream ss(line);
        string isbn, userIdStr;

        getline(ss, isbn, '|');
        isbn = trim(isbn);
        if (isbn == "") continue;

        Book* b = catalog.search(isbn);
        if (!b) continue;

        while (getline(ss, userIdStr, '|')) {
            int uid;
            if (!safeStoi(userIdStr, uid)) continue;
            b->waitlist.push(uid);
        }
    }
}

/* =========================================================
                       SAFE FILE SAVE FUNCTIONS
   ========================================================= */

void saveBooks(HashTable& catalog) {
    ofstream file("books.txt");
    auto list = catalog.getAllBooks();

    for (Book* b : list) {
        file << b->isbn << "|"
             << b->title << "|"
             << b->author << "|"
             << b->genre << "|"
             << b->totalCopies << "|"
             << b->availableCopies << "|"
             << b->popularityCount << "\n";
    }
}

void saveUsers(UserManager& users) {
    ofstream file("users.txt");
    for (int i = 0; i < users.userCount; i++) {
        file << users.userList[i].userId << "|"
             << users.userList[i].name << "\n";
    }
}

void saveHistory(UserManager& users) {
    ofstream file("history.txt");
    for (int i = 0; i < users.userCount; i++) {
        User& u = users.userList[i];
        file << u.userId;

        Node<string>* curr = u.history.getHead();
        while (curr) {
            file << "|" << curr->data;
            curr = curr->next;
        }
        file << "\n";
    }
}

void saveWaitlist(HashTable& catalog) {
    ofstream file("waitlist.txt");
    auto list = catalog.getAllBooks();

    for (Book* b : list) {
        file << b->isbn;

        QNode* curr = b->waitlist.getFront();
        while (curr) {
            file << "|" << curr->data;
            curr = curr->next;
        }
        file << "\n";
    }
}

/* =========================================================
                            MENUS
   ========================================================= */

void userMenu() {
    cout << "\n===== USER MENU =====\n";
    cout << "1. View All Books\n";
    cout << "2. Search Book\n";
    cout << "3. Borrow Book\n";
    cout << "4. Return Book\n";
    cout << "5. View Your History\n";
    cout << "6. Logout\n";
    cout << "Choose option: ";
}

void adminMenu() {
    cout << "\n===== ADMIN MENU =====\n";
    cout << "1. Add Book\n";
    cout << "2. View Full Book Inventory\n";
    cout << "3. Add User\n";
    cout << "4. View All Users\n";
    cout << "5. View User History\n";   // NEW
    cout << "6. Logout\n";
    cout << "Choose option: ";
}

/* =========================================================
                         MAIN PROGRAM
   ========================================================= */

int main() {

    HashTable catalog;
    UserManager users;
    BorrowEngine engine(catalog, users);
    PBRecommendationEngine popRec(catalog);
    BFSRecommendation bfsRec(catalog);
    GenreRecommendation genreRec(catalog);

    // Load safely BEFORE anything else
    loadBooks(catalog, genreRec);
    loadUsers(users);
    loadHistory(users, bfsRec);
    loadWaitlist(catalog);

    while (true) {
        cout << "\n===== LOGIN =====\n";
        cout << "1. Admin Login\n";
        cout << "2. User Login\n";
        cout << "3. Exit\n";
        cout << "Choose: ";
        int option;
        cin >> option;

        /* ADMIN LOGIN */
        if (option == 1) {
            string u, p;
            cout << "\nAdmin Username: ";
            cin >> u;
            cout << "Admin Password: ";
            cin >> p;

            if (u == "admin" && p == "1234") {
                cout << "\n[LOGIN SUCCESSFUL]\n";

                while (true) {
                    adminMenu();
                    int a; cin >> a;

                    if (a == 1) {
                        string isbn, title, author, genre;
                        int copies;

                        cout << "ISBN: ";
                        cin >> isbn;

                        cout << "Title: ";
                        cin.ignore();
                        getline(cin, title);

                        cout << "Author: ";
                        getline(cin, author);

                        cout << "Genre: ";
                        getline(cin, genre);

                        cout << "Total Copies: ";
                        cin >> copies;

                        Book b(isbn, title, author, copies, genre);
                        catalog.insert(b);
                        genreRec.addBook(genre, isbn);

                        saveBooks(catalog);
                        cout << "Book added.\n";
                    }

                    else if (a == 2) {
                        auto list = catalog.getAllBooks();
                        sort(list.begin(), list.end(),
                             [](Book* x, Book* y) { return x->title < y->title; });

                        cout << "\n===== BOOK INVENTORY =====\n";
                        for (auto b : list)
                            cout << b->title << " | ISBN: " << b->isbn
                                 << " | " << b->availableCopies << "/" << b->totalCopies
                                 << " copies | Popularity: " << b->popularityCount << "\n";
                    }

                    else if (a == 3) {
                        int id;
                        string name;
                        cout << "User ID: ";
                        cin >> id;
                        cout << "Name: ";
                        cin.ignore();
                        getline(cin, name);

                        bool ok = users.addUser(id, name);

                        if (!ok) {
                            cout << "\n[ERROR] User ID already exists! Choose a different ID.\n";
                        } 
                        else {
                            saveUsers(users);
                            cout << "\nUser added successfully!\n";
                        }

                    }

                    else if (a == 4) {
                        cout << "\n===== ALL USERS =====\n";
                        for (int i = 0; i < users.userCount; i++)
                            cout << users.userList[i].userId << " | "
                                 << users.userList[i].name << "\n";
                    }

                    else if (a == 5) {
                        int uid;
                        cout << "\nEnter User ID to view history: ";
                        cin >> uid;

                        User* u = users.getUser(uid);

                        if (!u) {
                            cout << "User not found.\n";
                        } 
                        else {
                            cout << "\n===== Borrowing History for " << u->name << " =====\n";
                            Node<string>* curr = u->history.getHead();
                            
                        if (!curr) {
                                cout << "(No borrow history)\n";
                            }

                        while (curr) {
                            Book* b = catalog.search(curr->data);
                            if (b) {
                                cout << "- " << b->title 
                                    << " (ISBN: " << b->isbn << ")\n";
                            }
                            curr = curr->next;
                        }
                            cout << "=========================================\n";
                        }
                    }
                    else if (a == 6) break;
                }
            }
            else cout << "\nInvalid admin login.\n";
        }

        /* USER LOGIN */
        else if (option == 2) {
            int uid;
            cout << "Enter User ID: ";
            cin >> uid;

            User* u = users.getUser(uid);
            if (!u) {
                cout << "User not found.\n";
                continue;
            }

            cout << "\nWelcome, " << u->name << "!\n";

            while (true) {
                userMenu();
                int c; cin >> c;

                if (c == 1) {
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(),
                         [](Book* a, Book* b) { return a->title < b->title; });

                    cout << "\n===== ALL BOOKS =====\n";
                    for (auto b : list)
                        cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                }

                else if (c == 2) {
                    string isbn;
                    cout << "ISBN: ";
                    cin >> isbn;

                    Book* b = catalog.search(isbn);
                    if (b)
                        cout << "Found: " << b->title << " by "
                             << b->author << "\n";
                    else
                        cout << "Not found.\n";
                }

                else if (c == 3) {
                    string isbn;
                    cout << "ISBN to borrow: ";
                    cin >> isbn;

                    Book* b = catalog.search(isbn);
                    if (!b) {
                        cout << "Not found.\n";
                        continue;
                    }

                    bool ok = engine.borrowBook(uid, isbn);

                    saveBooks(catalog);
                    saveHistory(users);
                    saveWaitlist(catalog);

                    if (ok) {
                        cout << "Borrowed successfully!\n";
                    }
                    else {
                        cout << "\nBook unavailable. Choose recommendation:\n";
                        cout << "1. Popularity\n";
                        cout << "2. BFS\n";
                        cout << "3. Genre\n";

                        int r; cin >> r;

                        if (r == 1) {
                            auto rec = popRec.getPopular(5);
                            for (auto x : rec)
                                cout << "- " << x->title << "\n";
                        }
                        else if (r == 2) bfsRec.traverse(isbn);
                        else if (r == 3) genreRec.recommend(b->genre);
                    }
                }

                else if (c == 4) {
                    string isbn;
                    cout << "ISBN to return: ";
                    cin >> isbn;

                    bool ok = engine.returnBook(uid, isbn);
                    saveBooks(catalog);
                    saveWaitlist(catalog);

                    if (ok) cout << "Returned.\n";
                }

                else if (c == 5) {
                    cout << "\n===== YOUR HISTORY =====\n";
                    Node<string>* curr = u->history.getHead();
                    while (curr) {
                        Book* b = catalog.search(curr->data);
                        if (b) cout << "- " << b->title << "\n";
                        curr = curr->next;
                    }
                }

                else if (c == 6) break;
            }
        }

        else if (option == 3) {
            saveBooks(catalog);
            saveUsers(users);
            saveHistory(users);
            saveWaitlist(catalog);
            cout << "Goodbye.\n";
            break;
        }
    }

    return 0;
}
