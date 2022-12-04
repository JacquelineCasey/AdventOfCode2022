
#include <iostream>

int main() {
    int a, b, c, d;
    char ch; // Throwaway

    int count {0};
    while (std::cin >> a >> ch >> b >> ch >> c >> ch >> d) {
        if ((a <= c && b >= d) || (c <= a && d >= b)) {
            count++;
        }
    }

    std::cout << count << '\n';
}
