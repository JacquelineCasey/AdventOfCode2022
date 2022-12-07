
#include <unordered_map>
#include <iostream>
#include <deque>

/* Our design is flexible. Change the 4's to 14's. */

int main() {
    std::unordered_map<char, int> counts {};
    std::deque<char> window{};

    for (int i = 0; i < 14; i++) {
        char ch;
        std::cin >> ch;
        window.push_back(ch);
        counts[ch]++; // Count defaults to 0 when inserted.
    } 

    int pos {14};

    while (counts.size() < 14) {
        // Add a new char
        pos++;
        char ch;
        std::cin >> ch;
        counts[ch]++;
        window.push_back(ch);

        // Remove an old char
        ch = window.front();
        window.pop_front();
        counts[ch]--;

        if (counts[ch] == 0) {
            counts.erase(ch);
        }
    }

    std::cout << pos << '\n';
}
