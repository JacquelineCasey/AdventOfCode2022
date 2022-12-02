
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

int main() {
    /* Why be sophisticated when you can use extra space? */
    std::vector<int> elf_sums {};
    int sum = 0;

    /* Getline returns the input stream. The input stream converts to false if
     * an error field is set (ie - the stream is finished). */
    for (std::string line; std::getline(std::cin, line); ) {
        if (line != "") {
            int item;
            std::stringstream(line) >> item;
            sum += item;   
        }
        else {
            elf_sums.push_back(sum);
            sum = 0;
        }
    }
    elf_sums.push_back(sum); // Last one.

    std::sort(elf_sums.begin(), elf_sums.end(), std::greater<int>()); // std::less is expected, so this reverses.

    std::cout << elf_sums[0] + elf_sums[1] + elf_sums[2] << '\n';
}
