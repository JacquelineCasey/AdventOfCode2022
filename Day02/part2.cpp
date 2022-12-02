
#include <iostream>


int get_score(char elf, char you) {
    /* It's hacky. Whatever. */
    int e = elf - 'A';
    int y = you - 'X';
    
    return y * 3 + (((e + y - 1 + 3) % 3) + 1);
    /* y * 3 for the round result. e + y - 1 tells you what shape you play. +3 
     * guarantees positive. mod 3 to get what you throw. Add 1 to adjust for points. */
}


int main() {
    char elf;
    char you;

    int score = 0;
    while (std::cin >> elf) {
        std::cin >> you;

        score += get_score(elf, you);
    }

    std::cout << score << '\n';
}  