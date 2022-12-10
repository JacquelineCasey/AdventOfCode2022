
#include <iostream>


int main() {
    int cycle = 0;
    int x = 1;

    int total = 0;
    std::string instruction {};
    while (std::cin >> instruction) {
        cycle++;

        if (cycle % 40 == 20) {
            total += x * cycle;
        } 

        // Its hacky but it works. noop will take 1 step. addx will take 2, 
        // and will add after then 2nd.
        if (instruction != "noop" && instruction != "addx") { 
            x += std::stoi(instruction);
        } 
    }

    std::cout << total << '\n';
}
