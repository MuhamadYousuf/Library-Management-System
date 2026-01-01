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
    cout << "4. View All Users (List)\n";
    cout << "5. View User Activity (History & Borrowed)\n"; // ✅ NEW
    cout << "6. Remove Book\n";           
    cout << "7. Report: Most Borrowed\n"; 
    cout << "8. Logout\n";
    cout << "Choose option: ";
}

void userMenu() {
    cout << "\n===== USER MENU =====\n";
    cout << "1. View All Books\n";
    cout << "2. Search Book\n";
    cout << "3. Borrow Book\n";
    cout << "4. Return Book\n";
    cout << "5. View Your Borrowed Books\n"; // ✅ NEW
    cout << "6. View Your History\n";
    cout << "7. Get Recommendations\n";      // ✅ NEW
    cout << "8. Logout\n";
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
    loadWaitlist(catalog);
    
    // Build graph for BFS
    bfsRec.buildGraph(users);

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
                    cout << "ISBN: "; cin >> isbn;
                    cout << "Title: "; cin.ignore(); getline(cin, title);
                    cout << "Author: "; getline(cin, author);
                    cout << "Genre: "; getline(cin, genre);
                    cout << "Total Copies: "; cin >> copies;

                    Book b(isbn, title, author, copies, genre);
                    catalog.insert(b);
                    genreRec.addBook(genre, isbn);
                    saveBooks(catalog);
                    cout << "Book added/updated.\n";
                }

                else if (a == 2) {
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(), [](Book* x, Book* y) { return x->title < y->title; });
                    cout << "\n===== BOOK INVENTORY =====\n";
                    for (auto b : list) {
                        cout << b->title << " | ISBN: " << b->isbn
                             << " | " << b->availableCopies << "/" << b->totalCopies
                             << " | Popularity: " << b->popularityCount << "\n";
                    }
                }

                else if (a == 3) {
                    int id; string name;
                    cout << "User ID: "; cin >> id;
                    cout << "Name: "; cin.ignore(); getline(cin, name);
                    if (!users.addUser(id, name)) cout << "[ERROR] User ID already exists.\n";
                    else { saveUsers(users); cout << "User added.\n"; }
                }

                else if (a == 4) {
                    cout << "\n===== ALL USERS =====\n";
                    for (int i = 0; i < users.userCount; i++) {
                        cout << users.userList[i].userId << " | " << users.userList[i].name << "\n";
                    }
                }

                // ✅ NEW: View User Activity (Admin)
                else if (a == 5) {
                    int uid;
                    cout << "Enter User ID to view activity: ";
                    cin >> uid;
                    User* target = users.getUser(uid);
                    
                    if (!target) {
                        cout << "User not found.\n";
                    } else {
                        cout << "\n--- Activity for " << target->name << " (" << target->userId << ") ---\n";
                        
                        cout << "[Currently Borrowed]:\n";
                        Node<string>* curr = target->borrowed.getHead();
                        if (!curr) cout << "  (None)\n";
                        while (curr) {
                            Book* b = catalog.search(curr->data);
                            if (b) cout << "  - " << b->title << " (ISBN: " << b->isbn << ")\n";
                            else   cout << "  - " << curr->data << "\n";
                            curr = curr->next;
                        }

                        cout << "[History]:\n";
                        curr = target->history.getHead();
                        if (!curr) cout << "  (None)\n";
                        while (curr) {
                            Book* b = catalog.search(curr->data);
                            if (b) cout << "  - " << b->title << " (ISBN: " << b->isbn << ")\n";
                            else   cout << "  - " << curr->data << "\n";
                            curr = curr->next;
                        }
                        cout << "------------------------------------------\n";
                    }
                }

                else if (a == 6) {
                    string isbn;
                    cout << "Enter ISBN to remove: "; cin >> isbn;
                    if (catalog.remove(isbn)) { saveBooks(catalog); cout << "Book removed.\n"; }
                    else cout << "Book not found.\n";
                }

                else if (a == 7) {
                    cout << "\n===== MOST BORROWED BOOKS =====\n";
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(), [](Book* x, Book* y) { 
                        return x->popularityCount > y->popularityCount; 
                    });
                    for (int i = 0; i < (int)list.size() && i < 5; i++) {
                        cout << i+1 << ". " << list[i]->title 
                             << " (Borrowed " << list[i]->popularityCount << " times)\n";
                    }
                }

                else if (a == 8) {
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
                    sort(list.begin(), list.end(), [](Book* a, Book* b) { return a->title < b->title; });
                    cout << "\n===== ALL BOOKS =====\n";
                    for (auto b : list) cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                }

                else if (c == 2) {
                    while (true) {
                        searchMenu();
                        int s; cin >> s;
                        if (s == 1) {
                            string isbn; cout << "Enter ISBN: "; cin >> isbn;
                            Book* b = catalog.search(isbn);
                            if (b) cout << "\nFound:\n" << b->title << " by " << b->author << "\n";
                            else cout << "Not found.\n";
                        }
                        else if (s == 2) {
                            Book* b = searchEngine.findBookInteractive();
                            if (b) cout << "\nFound:\n" << b->title << " by " << b->author << "\n";
                        }
                        else if (s == 3) break;
                    }
                }

                else if (c == 3) {
                    Book* selected[10];
                    int selCount = searchEngine.selectBooks(selected, 10, "borrow");
                    if (selCount > 0) {
                        for (int i = 0; i < selCount; i++) {
                            Book* b = selected[i];
                            bool ok = engine.borrowBook(uid, b->isbn);
                            saveBooks(catalog); saveHistory(users); saveBorrowed(users); saveWaitlist(catalog);
                            if (ok) cout << "Borrowed: " << b->title << "\n";
                            else {
                                if (u->borrowed.contains(b->isbn)) cout << "Already borrowed.\n";
                                else if (u->borrowed.count() >= 3) { /* error printed in engine */ }
                                else {
                                    cout << "Unavailable. Added to waitlist.\n";
                                    // Recommendation logic if unavailable
                                    cout << "Would you like recommendations? (1-Yes, 0-No): ";
                                    int r; cin >> r;
                                    if (r == 1) {
                                        cout << "1. Popular\n2. BFS (Similar users)\n3. Genre\nChoose: ";
                                        int m; cin >> m;
                                        if (m == 1) {
                                            auto rec = popRec.getPopular(5);
                                            for (auto x : rec) cout << "- " << x->title << "\n";
                                        } else if (m == 2) bfsRec.traverse(b->isbn);
                                        else if (m == 3) genreRec.recommend(b->genre);
                                    }
                                }
                            }
                        }
                    }
                }

                else if (c == 4) {
                    Book* selected[10];
                    int selCount = searchEngine.selectBooks(selected, 10, "return");
                    if (selCount > 0) {
                        for (int i = 0; i < selCount; i++) {
                            Book* b = selected[i];
                            bool ok = engine.returnBook(uid, b->isbn);
                            saveBooks(catalog); saveHistory(users); saveBorrowed(users); saveWaitlist(catalog);
                            if (ok) cout << "Returned: " << b->title << "\n";
                            else cout << "Error: You don't have this book.\n";
                        }
                    }
                }

                // ✅ NEW: View Borrowed Books
                else if (c == 5) {
                    cout << "\n===== YOUR BORROWED BOOKS =====\n";
                    Node<string>* curr = u->borrowed.getHead();
                    if (!curr) cout << "(You have no borrowed books)\n";
                    while (curr) {
                        Book* b = catalog.search(curr->data);
                        if (b) cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                        else   cout << "- " << curr->data << "\n";
                        curr = curr->next;
                    }
                }

                else if (c == 6) {
                    cout << "\n===== YOUR HISTORY =====\n";
                    Node<string>* curr = u->history.getHead();
                    if (!curr) cout << "(No history)\n";
                    while (curr) {
                        Book* b = catalog.search(curr->data);
                        if (b) cout << "- " << b->title << " (ISBN: " << b->isbn << ")\n";
                        else   cout << "- " << curr->data << "\n";
                        curr = curr->next;
                    }
                }

                // ✅ NEW: Main Menu Recommendation
                else if (c == 7) {
                    cout << "\n--- Get Recommendations ---\n";
                    cout << "1. Trending / Popular Books\n";
                    cout << "2. Similar to a book you like (BFS)\n";
                    cout << "3. By Genre\n";
                    cout << "4. Back\n";
                    cout << "Choose: ";
                    int m; cin >> m;

                    if (m == 1) {
                        cout << "\n[Top Trending Books]\n";
                        auto rec = popRec.getPopular(5);
                        for (auto x : rec) cout << "- " << x->title << "\n";
                    }
                    else if (m == 2) {
                        string isbn;
                        cout << "Enter ISBN of a book you liked: ";
                        cin >> isbn;
                        Book* b = catalog.search(isbn);
                        if (b) bfsRec.traverse(isbn);
                        else cout << "Book not found in catalog to base recommendations on.\n";
                    }
                    else if (m == 3) {
                        string g;
                        cout << "Enter Genre (e.g. Tech, SciFi, Fantasy): ";
                        cin.ignore(); getline(cin, g);
                        genreRec.recommend(g);
                    }
                }

                else if (c == 8) {
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