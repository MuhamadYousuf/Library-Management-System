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
#include "searchengine.h"s

// ---------------------------------------------------------
// UTILITY FUNCTIONS
// ---------------------------------------------------------
string trim(const string& s) {
    if (s.empty()) return "";
    int start = 0, end = (int)s.size() - 1;
    while (start <= end && (s[start] == ' ' || s[start] == '\t' || s[start] == '\r' || s[start] == '\n')) start++;
    while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' || s[end] == '\n')) end--;
    if (start > end) return "";
    return s.substr(start, end - start + 1);
}

void ensureFileExists(string filename) {
    ifstream f(filename);
    if (!f) { ofstream create(filename); create.close(); }
}

bool safeStoi(const string& s, int& out) {
    try {
        string t = trim(s);
        if (t.size() > 3 && (unsigned char)t[0] == 0xEF) t = t.substr(3); // Fix BOM
        if (t.empty()) return false;
        out = stoi(t);
        return true;
    } catch (...) { return false; }
}

// ---------------------------------------------------------
// LOAD FUNCTIONS
// ---------------------------------------------------------
void loadBooks(HashTable& catalog, GenreRecommendation& genreRec) {
    ensureFileExists("books.txt");
    ifstream file("books.txt");
    string line;
    while (getline(file, line)) {
        if (trim(line).empty()) continue;
        stringstream ss(line);
        string isbn, title, author, genre, total, available, pop;
        getline(ss, isbn, '|'); getline(ss, title, '|'); getline(ss, author, '|');
        getline(ss, genre, '|'); getline(ss, total, '|'); getline(ss, available, '|'); getline(ss, pop, '|');

        isbn = trim(isbn);
        if (isbn.empty()) continue;
        int t = 1, a = 1, p = 0;
        safeStoi(total, t); safeStoi(available, a); safeStoi(pop, p);

        Book b(isbn, trim(title), trim(author), t, trim(genre));
        b.availableCopies = a;
        b.popularityCount = p;
        catalog.insert(b);
        genreRec.addBook(trim(genre), isbn);
    }
}

void loadUsers(UserManager& users) {
    ensureFileExists("users.txt");
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        if (trim(line).empty()) continue;
        stringstream ss(line);
        string idStr, name;
        getline(ss, idStr, '|'); getline(ss, name, '|');
        int id;
        if (safeStoi(idStr, id)) users.addUser(id, trim(name));
    }
}

void loadHistory(UserManager& users) {
    ensureFileExists("history.txt");
    ifstream file("history.txt");
    string line;
    while (getline(file, line)) {
        if (trim(line).empty()) continue;
        stringstream ss(line);
        string idStr, isbn;
        if (!getline(ss, idStr, '|')) continue;
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
        if (trim(line).empty()) continue;
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
        if (trim(line).empty()) continue;
        stringstream ss(line);
        string isbn, uidStr;
        getline(ss, isbn, '|');
        isbn = trim(isbn);
        if (isbn == "") continue;
        Book* b = catalog.search(isbn);
        if (!b) continue;
        while (getline(ss, uidStr, '|')) {
            int uid;
            if (safeStoi(uidStr, uid) && !b->waitlist.contains(uid)) b->waitlist.enqueue(uid);
        }
    }
}

// ---------------------------------------------------------
// SAVE FUNCTIONS
// ---------------------------------------------------------
void saveAll(HashTable& catalog, UserManager& users) {
    // Save Books
    ofstream fb("books.txt");
    for (auto b : catalog.getAllBooks()) {
        fb << b->isbn << "|" << b->title << "|" << b->author << "|" << b->genre << "|"
           << b->totalCopies << "|" << b->availableCopies << "|" << b->popularityCount << "\n";
    }
    // Save Users
    ofstream fu("users.txt");
    for (int i = 0; i < users.userCount; i++) fu << users.userList[i].userId << "|" << users.userList[i].name << "\n";
    
    // Save History
    ofstream fh("history.txt");
    for (int i = 0; i < users.userCount; i++) {
        User& u = users.userList[i];
        fh << u.userId;
        Node<string>* curr = u.history.getHead();
        while (curr) { fh << "|" << curr->data; curr = curr->next; }
        fh << "\n";
    }

    // Save Borrowed
    ofstream fbr("borrowed.txt");
    for (int i = 0; i < users.userCount; i++) {
        User& u = users.userList[i];
        fbr << u.userId;
        Node<string>* curr = u.borrowed.getHead();
        while (curr) { fbr << "|" << curr->data; curr = curr->next; }
        fbr << "\n";
    }

    // Save Waitlist
    ofstream fw("waitlist.txt");
    for (auto b : catalog.getAllBooks()) {
        fw << b->isbn;
        QNode* curr = b->waitlist.getFront();
        while (curr) { fw << "|" << curr->data; curr = curr->next; }
        fw << "\n";
    }
}

// ---------------------------------------------------------
// MENUS
// ---------------------------------------------------------
void loginMenu() {
    cout << "\n===== LOGIN =====\n1. Admin Login\n2. User Login\n3. Exit\nChoose: ";
}
void adminMenu() {
    cout << "\n===== ADMIN MENU =====\n1. Add Book\n2. View Full Inventory\n3. Add User\n4. View Users\n5. View User Activity\n6. Remove Book\n7. Report: Most Borrowed\n8. Logout\nChoose: ";
}
void userMenu() {
    cout << "\n===== USER MENU =====\n1. View All Books\n2. Search Book\n3. Borrow Book\n4. Return Book\n5. View Borrowed Books\n6. View History\n7. Recommendations\n8. Logout\nChoose: ";
}

// ---------------------------------------------------------
// MAIN
// ---------------------------------------------------------
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
    bfsRec.buildGraph(users);

    while (true) {
        loginMenu();
        int option;
        if (!(cin >> option)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (option == 3) {
            saveAll(catalog, users);
            cout << "Goodbye.\n";
            break;
        }

        // --- ADMIN ---
        if (option == 1) {
            string u, p;
            cout << "Username: "; cin >> u;
            cout << "Password: "; cin >> p;
            if (u == "admin" && p == "1234") {
                while (true) {
                    adminMenu();
                    int a; cin >> a;
                    if (a == 8) break;
                    
                    if (a == 1) { // Add Book
                        string isbn, title, author, genre; int copies;
                        cout << "ISBN: "; cin >> isbn;
                        cout << "Title: "; cin.ignore(); getline(cin, title);
                        cout << "Author: "; getline(cin, author);
                        cout << "Genre: "; getline(cin, genre);
                        cout << "Total Copies: "; cin >> copies;
                        Book b(isbn, title, author, copies, genre);
                        catalog.insert(b); genreRec.addBook(genre, isbn);
                        saveAll(catalog, users);
                        cout << "Book added.\n";
                    }
                    else if (a == 2) { // View Inventory
                        auto list = catalog.getAllBooks();
                        sort(list.begin(), list.end(), [](Book* x, Book* y) { return x->title < y->title; });
                        cout << "\n--- INVENTORY ---\n";
                        for (auto b : list) cout << b->title << " | ISBN: " << b->isbn << " | " << b->availableCopies << "/" << b->totalCopies << "\n";
                    }
                    else if (a == 3) { // Add User
                        int id; string name;
                        cout << "ID: "; cin >> id; cout << "Name: "; cin.ignore(); getline(cin, name);
                        if (users.addUser(id, name)) { saveAll(catalog, users); cout << "User added.\n"; }
                        else cout << "ID exists.\n";
                    }
                    else if (a == 4) { // View Users
                        for(int i=0; i<users.userCount; i++) cout << users.userList[i].userId << " | " << users.userList[i].name << "\n";
                    }
                    else if (a == 5) { // User Activity
                        int uid; cout << "User ID: "; cin >> uid;
                        User* t = users.getUser(uid);
                        if(t) {
                            cout << "\n[Borrowed]:\n";
                            Node<string>* c = t->borrowed.getHead();
                            while(c) { 
                                Book* b = catalog.search(c->data);
                                cout << "- " << (b ? b->title : c->data) << "\n"; 
                                c = c->next; 
                            }
                            cout << "[History]:\n";
                            c = t->history.getHead();
                            while(c) {
                                Book* b = catalog.search(c->data);
                                cout << "- " << (b ? b->title : c->data) << "\n"; 
                                c = c->next; 
                            }
                        } else cout << "Not found.\n";
                    }
                    else if (a == 6) { // Remove Book
                        string isbn; cout << "ISBN: "; cin >> isbn;
                        if(catalog.remove(isbn)) { saveAll(catalog, users); cout << "Removed.\n"; }
                        else cout << "Not found.\n";
                    }
                    else if (a == 7) { // Most Borrowed
                        auto list = catalog.getAllBooks();
                        sort(list.begin(), list.end(), [](Book* x, Book* y) { return x->popularityCount > y->popularityCount; });
                        for(int i=0; i<min((int)list.size(), 5); i++) cout << i+1 << ". " << list[i]->title << " (" << list[i]->popularityCount << " borrows)\n";
                    }
                }
            } else cout << "Invalid login.\n";
        }

        // --- USER ---
        else if (option == 2) {
            int uid; cout << "User ID: "; cin >> uid;
            User* u = users.getUser(uid);
            if (!u) { cout << "User not found.\n"; continue; }
            cout << "\nWelcome " << u->name << "!\n";

            while (true) {
                userMenu();
                int c; cin >> c;
                if (c == 8) break;

                if (c == 1) { // View All
                    auto list = catalog.getAllBooks();
                    sort(list.begin(), list.end(), [](Book* a, Book* b) { return a->title < b->title; });
                    for(auto b : list) cout << "- " << b->title << " (" << b->isbn << ")\n";
                }
                else if (c == 2) { // Search
                    Book* b = searchEngine.findBookInteractive();
                    if(b) cout << "\nFound: " << b->title << " | " << b->author << "\n";
                }
                else if (c == 3) { // Borrow
                    Book* selected[10];
                    int n = searchEngine.selectBooks(selected, 10, "borrow");
                    for(int i=0; i<n; i++) {
                        if(engine.borrowBook(uid, selected[i]->isbn)) cout << "Borrowed: " << selected[i]->title << "\n";
                        else if (u->borrowed.contains(selected[i]->isbn)) cout << "Already have it.\n";
                        else if (u->borrowed.count() >= 3) cout << "Limit reached (Max 3).\n";
                        else {
                            cout << "Waitlisted for " << selected[i]->title << "\n";
                            // Recs...
                        }
                    }
                    saveAll(catalog, users);
                }
                // ✅ OPTIMIZED RETURN FLOW
                else if (c == 4) { 
                    Node<string>* curr = u->borrowed.getHead();
                    if (!curr) {
                        cout << "\nYou have no borrowed books to return.\n";
                    } else {
                        cout << "\nSelect a book to return:\n";
                        vector<string> bookIsbns;
                        int index = 1;
                        while (curr) {
                            Book* b = catalog.search(curr->data);
                            cout << index << ". " << (b ? b->title : "Unknown Title") 
                                 << " (ISBN: " << curr->data << ")\n";
                            bookIsbns.push_back(curr->data);
                            curr = curr->next;
                            index++;
                        }
                        
                        cout << "Enter number (0 to cancel): ";
                        int choice;
                        cin >> choice;
                        
                        if (choice > 0 && choice <= (int)bookIsbns.size()) {
                            string isbnToReturn = bookIsbns[choice - 1];
                            if (engine.returnBook(uid, isbnToReturn)) {
                                saveAll(catalog, users);
                                cout << "Returned successfully.\n";
                            } else {
                                cout << "Error returning book.\n";
                            }
                        }
                    }
                }
                else if (c == 5) { // View Borrowed
                    Node<string>* curr = u->borrowed.getHead();
                    if(!curr) cout << "None.\n";
                    while(curr) {
                        Book* b = catalog.search(curr->data);
                        cout << "- " << (b ? b->title : curr->data) << "\n";
                        curr = curr->next;
                    }
                }
                else if (c == 6) { // View History
                    Node<string>* curr = u->history.getHead();
                    if(!curr) cout << "None.\n";
                    while(curr) {
                        Book* b = catalog.search(curr->data);
                        cout << "- " << (b ? b->title : curr->data) << "\n";
                        curr = curr->next;
                    }
                }
                else if (c == 7) { // Recs
                    cout << "1. Popular\n2. BFS\n3. Genre\nChoose: ";
                    int m; cin >> m;
                    if(m==1) {
                        for(auto x : popRec.getPopular(5)) cout << "- " << x->title << "\n";
                    } else if (m==2) {
                        string isbn; cout << "Enter ISBN you liked: "; cin >> isbn;
                        bfsRec.traverse(isbn);
                    } else if (m==3) {
                        string g; cout << "Genre: "; cin.ignore(); getline(cin, g);
                        genreRec.recommend(g);
                    }
                }
            }
        }
    }
    return 0;
}