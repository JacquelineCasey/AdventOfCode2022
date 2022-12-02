
#include <iostream>


int get_score(char elf, char you) {
    /* It's hacky. Whatever. */
    int e = elf - 'A';
    int y = you - 'X';
    
    if (y - e == 1 || y - e == -2) { // Win
        return y + 1 + 6;
    }
    else if (y - e == 0) { // Tie
        return y + 1 + 3;
    }
    else { // Lose
        return y + 1;
    }
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