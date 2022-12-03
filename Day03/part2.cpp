
#include <iostream>
#include <unordered_set>

int priority(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 1;
    }
    else if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 26 + 1;
    }
    else {
        std::cout << "Something went wrong.\n";
        return -1;
    }
}

int get_intersection_priority(const std::string& first, const std::string& second, const std::string& third) {
    std::unordered_set<char> first_set {};
    std::unordered_set<char> second_set {}; // actually the intersection of first and second.

    for (char ch : first) {
        first_set.insert(ch);
    }

    for (char ch : second) {
        if (first_set.contains(ch)) { // C++20 finally added contains. Otherwise - find() and compare to end()
            second_set.insert(ch);
        } 
    }
    
    for (char ch : third) {
        if (second_set.contains(ch)) {
            return priority(ch);
        }
    }

    std::cout << "Something went wrong.\n";
    return -1;

    /* Note: std::set_intersection exists, though I believe would only work on
     * the sorted std::set (or any sorted range, for that matter). */
}

int main() {
    int sum {0};

    std::string first {};
    std::string second {};
    std::string third {};
    while (std::cin >> first >> second >> third) {
        sum += get_intersection_priority(first, second, third);
    }

    std::cout << sum << '\n';
}
