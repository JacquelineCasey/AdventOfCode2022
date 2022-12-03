
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

int get_intersection_priority(const std::string& left, const std::string& right) {
    std::unordered_set<char> left_set {};

    for (char ch : left) {
        left_set.insert(ch);
    }

    for (char ch : right) {
        if (left_set.contains(ch)) { // C++20 finally added contains(). Otherwise - find() and compare to end()
            return priority(ch);
        } 
    }

    std::cout << "Something went wrong.\n";
    return -1;
}

int main() {
    int sum {0};

    std::string buffer {};
    while (std::cin >> buffer) {
        std::string left {buffer.substr(0, buffer.length() / 2)};
        std::string right {buffer.substr(buffer.length() / 2)};

        sum += get_intersection_priority(left, right);
    }

    std::cout << sum << '\n';
}
