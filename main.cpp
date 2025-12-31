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
#include "searchengine.h"

/* =========================================================
                 UTILITY: TRIM WHITESPACE
   ========================================================= */
string trim(const string& s) {
    int start = 0, end = (int)s.size() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t')) start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t')) end--;
    return s.substr(start, end - start + 1);
}

/* =========================================================
                       SAFE FILE HELPERS
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
        string t = trim(s);
        if (t.empty()) return false;
        out = stoi(t);
        return true;
    } catch (...) {
        return false;
    }
}

/* =========================================================
                       LOAD FUNCTIONS
   ========================================================= */
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

        users.addUser(id, trim(name));
    }
}

void loadHistory(UserManager& users) {
    ensureFileExists("history.txt");
    ifstream file("history.txt");

    string line;
    while (getline(file, line)) {
        if (line == "") continue;

        stringstream ss(line);
        string idStr, isbn;

        getline(ss, idStr, '|');
        int userId;
        if (!safeStoi(idStr, userId)) continue;

        User* u = users.getUser(userId);
        if (!u) continue;

        while (getline(ss, isbn, '|')) {
            isbn = trim(isbn);
            if (!isbn.empty()) u->history.insert(isbn);
        }
    }
}

void loadBorrowed(UserManager& users) {
    ensureFileExists("borrowed.txt");
    ifstream file("borrowed.txt");

    string line;
    while (getline(file, line)) {
        if (line == "") continue;

        stringstream ss(line);
        string idStr, isbn;

        getline(ss, idStr, '|');
        int userId;
        if (!safeStoi(idStr, userId)) continue;

        User* u = users.getUser(userId);
        if (!u) continue;

        while (getline(ss, isbn, '|')) {
            isbn = trim(isbn);
            if (!isbn.empty()) u->borrowed.insert(isbn);
        }
    }
}

// ✅ WAITLIST LOAD (the missing one)
void loadWaitlist(HashTable& catalog) {
    ensureFileExists("waitlist.txt");
    ifstream file("waitlist.txt");

    string line;
    while (getline(file, line)) {
        if (line == "") continue;

        stringstream ss(line);
        string isbn, uidStr;

        getline(ss, isbn, '|');
        isbn = trim(isbn);
        if (isbn == "") continue;

        Book* b = catalog.search(isbn);
        if (!b) continue;

        while (getline(ss, uidStr, '|')) {
            int uid;
            if (!safeStoi(uidStr, uid)) continue;

            // avoid duplicates in queue
            if (!b->waitlist.contains(uid)) {
                b->waitlist.enqueue(uid);
            }
        }
    }
}

/* =========================================================
                       SAVE FUNCTIONS
   ========================================================= */
void saveBooks(HashTable& catalog) {
    ofstream file("books.txt");
    auto list = catalog.getAllBooks();

    for (auto b : list) {
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

void saveBorrowed(UserManager& users) {
    ofstream file("borrowed.txt");
    for (int i = 0; i < users.userCount; i++) {
        User& u = users.userList[i];
        file << u.userId;

        Node<string>* curr = u.borrowed.getHead();
        while (curr) {
            file << "|" << curr->data;
            curr = curr->next;
        }
        file << "\n";
    }
}

// ✅ WAITLIST SAVE
void saveWaitlist(HashTable& catalog) {
    ofstream file("waitlist.txt");
    auto list = catalog.getAllBooks();

    for (auto b : list) {
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
void loginMenu() {
    cout << "\n===== LOGIN =====\n";
    cout << "1. Admin Login\n";
    cout << "2. User Login\n";
    cout << "3. Exit\n";
    cout << "Choose: ";
}

void adminMenu() {
    cout << "\n===== ADMIN MENU =====\n";
    cout << "1. Add Book\n";
    cout << "2. View Full Book Inventory\n";
    cout << "3. Add User\n";
    cout << "4. View All Users\n";
    cout << "5. Logout\n";
    cout << "Choose option: ";
}

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

void searchMenu() {
    cout << "\n--- Search Options ---\n";
    cout << "1. Search by ISBN\n";
    cout << "2. Search by Title / Author\n";
    cout << "3. Back\n";
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
    SearchEngine searchEngine(catalog);

    loadBooks(catalog, genreRec);
    loadUsers(users);
    loadBorrowed(users);
    loadHistory(users);
    loadWaitlist(catalog);   // ✅ now included

    while (true) {
        loginMenu();
        int option;
        cin >> option;

        if (option == 3) {
            saveBooks(catalog);
            saveUsers(users);
            saveHistory(users);
            saveBorrowed(users);
            saveWaitlist(catalog);
            cout << "Goodbye.\n";
            break;
        }

        /* ---------------- ADMIN LOGIN ---------------- */
        if (option == 1) {
            string u, p;
            cout << "\nAdmin Username: ";
            cin >> u;
            cout << "Admin Password: ";
            cin >> p;

            if (!(u == "admin" && p == "1234")) {
                cout << "\nInvalid admin login.\n";
                continue;
            }

            cout << "\n[ADMIN LOGIN SUCCESSFUL]\n";

            while (true) {
                adminMenu();
                int a;
                cin >> a;

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
                    cout << "Book added/updated.\n";
                }

                else if (a == 2) {
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(),
                         [](Book* x, Book* y) { return x->title < y->title; });

                    cout << "\n===== BOOK INVENTORY =====\n";
                    for (auto b : list) {
                        cout << b->title << " | ISBN: " << b->isbn
                             << " | " << b->availableCopies << "/" << b->totalCopies
                             << " | Popularity: " << b->popularityCount << "\n";
                    }
                }

                else if (a == 3) {
                    int id;
                    string name;

                    cout << "User ID: ";
                    cin >> id;

                    cout << "Name: ";
                    cin.ignore();
                    getline(cin, name);

                    if (!users.addUser(id, name)) {
                        cout << "[ERROR] User ID already exists.\n";
                    } else {
                        saveUsers(users);
                        cout << "User added.\n";
                    }
                }

                else if (a == 4) {
                    cout << "\n===== ALL USERS =====\n";
                    for (int i = 0; i < users.userCount; i++) {
                        cout << users.userList[i].userId << " | "
                             << users.userList[i].name << "\n";
                    }
                }

                else if (a == 5) {
                    cout << "[ADMIN LOGOUT]\n";
                    break;
                }

                else cout << "Invalid option.\n";
            }
        }

        /* ---------------- USER LOGIN ---------------- */
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
                int c;
                cin >> c;

                if (c == 1) {
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(),
                         [](Book* a, Book* b) { return a->title < b->title; });

                    cout << "\n===== ALL BOOKS =====\n";
                    for (auto b : list)
                        cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                }

                else if (c == 2) {
                    while (true) {
                        searchMenu();
                        int s;
                        cin >> s;

                        if (s == 1) {
                            string isbn;
                            cout << "Enter ISBN: ";
                            cin >> isbn;

                            Book* b = catalog.search(isbn);
                            if (b) {
                                cout << "\nFound:\n" << b->title << " by " << b->author << "\n";
                            } else {
                                cout << "Book not found by ISBN.\nTry Title/Author search.\n";
                            }
                        }
                        else if (s == 2) {
                            Book* b = searchEngine.findBookInteractive();
                            if (b) cout << "\nFound:\n" << b->title << " by " << b->author << "\n";
                        }
                        else if (s == 3) break;
                        else cout << "Invalid option.\n";
                    }
                }

                else if (c == 3) {
                    Book* selected[10];
                    int selCount = searchEngine.selectBooks(selected, 10, "borrow");
                    if (selCount == 0) {
                        cout << "No book selected.\n";
                        continue;
                    }

                    for (int i = 0; i < selCount; i++) {
                        Book* b = selected[i];

                        bool ok = engine.borrowBook(uid, b->isbn);

                        // ✅ save everything related
                        saveBooks(catalog);
                        saveHistory(users);
                        saveBorrowed(users);
                        saveWaitlist(catalog);

                        if (ok) {
                            cout << "Borrowed successfully: " << b->title
                                 << " (ISBN: " << b->isbn << ")\n";
                        } else {
                            // could be: already borrowed OR waitlisted
                            if (u->borrowed.contains(b->isbn)) {
                                cout << "[INFO] You already borrowed this book.\n";
                            } else {
                                cout << "[INFO] Book unavailable. You are in waitlist (if not already).\n";
                                cout << "Recommendations:\n1. Popularity\n2. BFS\n3. Genre\nChoose: ";
                                int r; cin >> r;
                                if (r == 1) {
                                    auto rec = popRec.getPopular(5);
                                    for (auto x : rec) cout << "- " << x->title << "\n";
                                } else if (r == 2) bfsRec.traverse(b->isbn);
                                else if (r == 3) genreRec.recommend(b->genre);
                            }
                        }
                    }
                }

                else if (c == 4) {
                    // return by searching (engine checks if user has it)
                    Book* selected[10];
                    int selCount = searchEngine.selectBooks(selected, 10, "return");
                    if (selCount == 0) {
                        cout << "No book selected.\n";
                        continue;
                    }

                    for (int i = 0; i < selCount; i++) {
                        Book* b = selected[i];

                        bool ok = engine.returnBook(uid, b->isbn);

                        saveBooks(catalog);
                        saveHistory(users);
                        saveBorrowed(users);
                        saveWaitlist(catalog);

                        if (ok) {
                            cout << "Returned successfully: " << b->title
                                 << " (ISBN: " << b->isbn << ")\n";
                        } else {
                            cout << "[ERROR] You cannot return this book (not borrowed).\n";
                        }
                    }
                }

                else if (c == 5) {
                    cout << "\n===== YOUR HISTORY =====\n";
                    Node<string>* curr = u->history.getHead();
                    if (!curr) cout << "(No history)\n";

                    while (curr) {
                        Book* b = catalog.search(curr->data);
                        if (b) cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                        else cout << "- " << curr->data << " (Unknown ISBN)\n";
                        curr = curr->next;
                    }
                }

                else if (c == 6) {
                    cout << "[USER LOGOUT]\n";
                    break;
                }

                else cout << "Invalid option.\n";
            }
        }

        else {
            cout << "Invalid login option.\n";
        }
    }

    return 0;
}
