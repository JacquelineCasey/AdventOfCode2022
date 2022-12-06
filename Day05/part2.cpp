
#include <iostream>
#include <sstream>
#include <vector>

struct command {
public:
    int count;
    int from;
    int to;
};

std::istream& operator>>(std::istream& in, command& command) {
    std::string buffer {};

    return in >> buffer >> command.count >> buffer >> command.from >> buffer >> command.to;
}

void run_command(std::vector<std::vector<char>>& stacks, const command& command) {
    /* Only changes... */

    std::vector<char> temp {};

    for (int i {0}; i < command.count; i++) {
        temp.push_back(stacks[command.from - 1].back());
        stacks[command.from - 1].pop_back();
    }

    for (int i {0}; i < command.count; i++) {
        stacks[command.to - 1].push_back(temp.back());
        temp.pop_back();
    }
}

int main() {
    /* Initial Parsing. */

    std::vector<std::string> lines {};

    while (true) {
        std::string str {};
        std::getline(std::cin, str);
        if (str == "") {
            break;
        }
        lines.push_back(str);
    }

    std::vector<command> commands {};

    command temp {};
    while (std::cin >> temp) {
        commands.push_back(temp);
    }

    /* Get number of stacks. */

    std::stringstream base_line {lines.back()}; // Remove baseline to buffer.
    lines.pop_back();

    int stack_num {0};
    int i_buf;
    while (base_line >> i_buf) {
        stack_num++;
    }

    // Will be numbered 0 to stack_num - 1
    std::vector<std::vector<char>> stacks (stack_num, std::vector<char>());

    /* Process input from bottom to top. */

    while (!lines.empty()) {
        std::string line {lines.back()};
        lines.pop_back();

        int col_num {0};
        for (int i = 1; i < static_cast<int>(line.size()); i += 4) {
            if (line[i] != ' ') {
                stacks[col_num].push_back(line[i]);
            }
            col_num++;
        }
    }
    
    for (const command& command : commands) {
        run_command(stacks, command);
    }

    for (int i {0}; i < stack_num; i++) {
        std::cout << stacks[i].back();
    }

    std::cout << '\n';
}


