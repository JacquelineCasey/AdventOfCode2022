
#include <iostream>

int main() {
    int a, b, c, d;
    char ch; // Throwaway

    int count {0};
    while (std::cin >> a >> ch >> b >> ch >> c >> ch >> d) {
        if (!(b < c || d < a)) {
            count++;
        }
    }

    std::cout << count << '\n';   
}
