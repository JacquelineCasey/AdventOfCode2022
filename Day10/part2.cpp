
#include <iostream>


int main() {
    int cycle = 0;
    int x = 1;

    std::string instruction {};
    while (std::cin >> instruction) {
        
        // Output # or space
        int horizontal_pos = cycle % 40;
        if (horizontal_pos == x - 1 || horizontal_pos == x || horizontal_pos == x + 1) {
            std::cout << '#';
        }
        else {
            std::cout << ' ';
        }

        // Newline after 40
        if (cycle % 40 == 39) {
            std::cout << '\n';
        }

        cycle++;

        if (instruction != "noop" && instruction != "addx") {
            x += std::stoi(instruction);
        } 
    }
}
