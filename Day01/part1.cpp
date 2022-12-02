
#include <iostream>
#include <string>
#include <sstream>

int main() {
    int running_max {-1};
    int elf_sum = 0;

    /* Getline returns the input stream. The input stream converts to false if
     * an error field is set (ie - the stream is finished). */
    for (std::string line; std::getline(std::cin, line); ) {
        if (line != "") {
            int item;
            std::stringstream(line) >> item;
            elf_sum += item;   
        }
        else {
            running_max = std::max(running_max, elf_sum);
            elf_sum = 0;
        }
    }
    running_max = std::max(running_max, elf_sum); // Last one.

    std::cout << running_max << '\n';
}
