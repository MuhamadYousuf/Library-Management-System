#ifndef DATASEEDER_H
#define DATASEEDER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

class DataSeeder {
public:
    static void generate() {
        srand(time(0)); // Seed random number generator

        // 1. GENERATE BOOKS (4 Genres, 5 Books each = 20 Books)
        // ISBNs will be 100 to 119
        ofstream bookFile("books.txt");
        if (!bookFile) return;

        struct BookData { string t; string a; string g; };
        vector<BookData> library;

        // Tech Books (IDs 100-104)
        library.push_back({"The C++ Language", "Bjarne Stroustrup", "Tech"});
        library.push_back({"Clean Code", "Robert C. Martin", "Tech"});
        library.push_back({"Pragmatic Programmer", "Andrew Hunt", "Tech"});
        library.push_back({"Design Patterns", "Erich Gamma", "Tech"});
        library.push_back({"Intro to Algorithms", "Thomas Cormen", "Tech"});

        // Sci-Fi Books (IDs 105-109)
        library.push_back({"Dune", "Frank Herbert", "SciFi"});
        library.push_back({"Ender's Game", "Orson Scott Card", "SciFi"});
        library.push_back({"Foundation", "Isaac Asimov", "SciFi"});
        library.push_back({"Neuromancer", "William Gibson", "SciFi"});
        library.push_back({"Snow Crash", "Neal Stephenson", "SciFi"});

        // Fantasy Books (IDs 110-114)
        library.push_back({"Harry Potter", "J.K. Rowling", "Fantasy"});
        library.push_back({"The Hobbit", "J.R.R. Tolkien", "Fantasy"});
        library.push_back({"Game of Thrones", "G.R.R. Martin", "Fantasy"});
        library.push_back({"Name of the Wind", "Patrick Rothfuss", "Fantasy"});
        library.push_back({"Way of Kings", "Brandon Sanderson", "Fantasy"});

        // History Books (IDs 115-119)
        library.push_back({"Sapiens", "Yuval Noah Harari", "History"});
        library.push_back({"Guns Germs Steel", "Jared Diamond", "History"});
        library.push_back({"Genghis Khan", "Jack Weatherford", "History"});
        library.push_back({"The Silk Roads", "Peter Frankopan", "History"});
        library.push_back({"1776", "David McCullough", "History"});

        for (int i = 0; i < library.size(); i++) {
            int isbn = 100 + i;
            // Format: ISBN|Title|Author|Genre|Total|Available|Popularity
            // Randomize popularity between 5 and 50 to make reports interesting
            int pop = rand() % 45 + 5; 
            bookFile << isbn << "|" << library[i].t << "|" << library[i].a << "|" 
                     << library[i].g << "|5|5|" << pop << "\n";
        }
        bookFile.close();
        cout << "[DataSeeder] Generated 20 Books.\n";

        // 2. GENERATE USERS (15 Users)
        ofstream userFile("users.txt");
        vector<string> names = {"Alice", "Bob", "Charlie", "Dave", "Eve", "Frank", "Grace", "Heidi", "Ivan", "Judy", "Mallory", "Niaj", "Olivia", "Peggy", "Sybil"};
        
        for (int i = 0; i < names.size(); i++) {
            userFile << (1001 + i) << "|" << names[i] << "\n";
        }
        userFile.close();
        cout << "[DataSeeder] Generated 15 Users.\n";

        // 3. GENERATE HISTORY (Dense Connections)
        // Logic: Users 0-3 love Tech, 4-7 love SciFi, etc.
        // But they also borrow 1 random book from another genre to create "Bridges".
        ofstream histFile("history.txt");
        
        for (int i = 0; i < names.size(); i++) {
            int userId = 1001 + i;
            histFile << userId;

            // Determine "Favorite Genre" based on user index
            int startISBN, endISBN;
            if (i < 4) { startISBN = 100; endISBN = 104; }      // Tech Fans
            else if (i < 8) { startISBN = 105; endISBN = 109; } // SciFi Fans
            else if (i < 12) { startISBN = 110; endISBN = 114; }// Fantasy Fans
            else { startISBN = 115; endISBN = 119; }            // History Fans

            // Borrow 3 books from FAVORITE genre
            for (int k = 0; k < 3; k++) {
                int book = startISBN + (rand() % 5);
                histFile << "|" << book;
            }

            // Borrow 1 book from RANDOM genre (The Bridge)
            // This ensures the graph is connected!
            int randomBook = 100 + (rand() % 20);
            histFile << "|" << randomBook;

            histFile << "\n";
        }
        histFile.close();
        cout << "[DataSeeder] Generated Connected History Data.\n";

        // 4. CLEAR OTHER FILES
        ofstream f1("borrowed.txt"); f1.close();
        ofstream f2("waitlist.txt"); f2.close();
    }
};

#endif