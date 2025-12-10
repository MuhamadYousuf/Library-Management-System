#include <iostream>
#include <string>
#include "hashtable.h"
#include "usermanager.h"
#include "borrowengine.h"

using namespace std;

void menu() {
    cout << "\n===== SMART LIBRARY SYSTEM =====\n";
    cout << "1. Add Book\n";
    cout << "2. Search Book\n";
    cout << "3. Add User\n";
    cout << "4. Borrow Book\n";
    cout << "5. Return Book\n";
    cout << "6. View User History\n";
    cout << "7. Exit\n";
    cout << "Choose option: ";
}

int main() {
    HashTable catalog;
    UserManager users;
    BorrowEngine engine(catalog, users);

    int choice;

    while (true) {
        menu();
        cin >> choice;

        if (choice == 1) {
            string isbn, title, author;
            int copies;

            cout << "\nEnter ISBN: ";
            cin >> isbn;
            cout << "Enter Title: ";
            cin.ignore();
            getline(cin, title);
            cout << "Enter Author: ";
            getline(cin, author);
            cout << "Enter Total Copies: ";
            cin >> copies;

            catalog.insert(Book(isbn, title, author, copies));
            cout << "Book added successfully!\n";
        }

        else if (choice == 2) {
            string isbn;
            cout << "\nEnter ISBN to search: ";
            cin >> isbn;

            Book* b = catalog.search(isbn);
            if (b) {
                cout << "\nBook Found:\n";
                cout << "Title: " << b->title << endl;
                cout << "Author: " << b->author << endl;
                cout << "Available Copies: " << b->availableCopies << endl;
            } else {
                cout << "Book not found.\n";
            }
        }

        else if (choice == 3) {
            int id;
            string name;

            cout << "\nEnter User ID: ";
            cin >> id;

            cout << "Enter User Name: ";
            cin.ignore();
            getline(cin, name);

            users.addUser(id, name);
            cout << "User added successfully!\n";
        }

        else if (choice == 4) {
            int userId;
            string isbn;

            cout << "\nEnter User ID: ";
            cin >> userId;
            cout << "Enter ISBN to borrow: ";
            cin >> isbn;

            bool success = engine.borrowBook(userId, isbn);
            if (success)
                cout << "Book borrowed successfully!\n";
            else
                cout << "No copies available. User added to waitlist.\n";
        }

        else if (choice == 5) {
            int userId;
            string isbn;

            cout << "\nEnter User ID: ";
            cin >> userId;
            cout << "Enter ISBN to return: ";
            cin >> isbn;

            bool success = engine.returnBook(userId, isbn);
            if (success)
                cout << "Book returned successfully!\n";
            else
                cout << "Error returning book.\n";
        }

        else if (choice == 6) {
            int userId;
            cout << "\nEnter User ID: ";
            cin >> userId;

            User* u = users.getUser(userId);
            if (!u) {
                cout << "User not found.\n";
            } else {
                cout << "\nBorrowing History for " << u->name << ":\n";

                Node<string>* curr = u->history.getHead();

                while (curr != nullptr) {
                    Book* b = catalog.search(curr->data);

                    if (b) {
                        cout << b->title << " (ISBN: " << b->isbn << ")\n";
                    } else {
                        cout << curr->data << " (Not found in catalog)\n";
                    }

                    curr = curr->next;
                }

                cout << "End of history.\n";
            }
        }


        else if (choice == 7) {
            cout << "Exiting system...\n";
            break;
        }

        else {
            cout << "Invalid option! Try again.\n";
        }
    }

    return 0;
}
