
#include <deque>
#include <iostream>
#include <vector>


struct monkey {
public:
    std::deque<long long> items {};
    char op;
    std::string operand; // A number, or "old"
    int test_mod;
    int true_toss;
    int false_toss;

    int inspect_count {0};

    friend std::istream& operator>>(std::istream& in, monkey& monkey);
};

std::istream& operator>>(std::istream& in, monkey& monkey) {
    monkey = {}; // RESET the monkey (this is a pain to remember!)

    std::string buf{};
    in >> buf >> buf; // First line
    in >> buf >> buf; // Starting items:

    if (!in) {
        return in;
    }

    int item;
    while (true) {
        in >> item;
        monkey.items.push_back(item);
        in >> buf;
        if (buf == "Operation:") {
            break;
        }
    }

    in >> buf >> buf >> buf; // new = old 
    in >> monkey.op;
    in >> monkey.operand;

    in >> buf >> buf >> buf; // Test: divisible by
    in >> monkey.test_mod;

    in >> buf >> buf >> buf >> buf >> buf; // If true: throw to monkey
    in >> monkey.true_toss;

    in >> buf >> buf >> buf >> buf >> buf; // If false: throw to monkey
    in >> monkey.false_toss;

    // std::cout << "{op: " << monkey.op << " operand: " << monkey.operand << " true_toss: " << monkey.true_toss << " false_toss: " << monkey.false_toss << "}\n";

    return in;
}

void process_monkey(std::vector<monkey>& monkeys, int i) {
    while (!monkeys[i].items.empty()) {
        long long item {monkeys[i].items.front()};
        monkeys[i].items.pop_front();

        // std::cout << "Monkey " << i << " with " << item << '\n';

        long long operand {0};
        if (monkeys[i].operand == "old") {
            operand = item;
        }
        else {
            operand = std::stoi(monkeys[i].operand);
        }

        long long new_item;
        if (monkeys[i].op == '+') {
            new_item = item + operand;
        }
        else if (monkeys[i].op == '*') {
            new_item = item * operand;
        }
        else {
            std::cout << "Something went wrong\n";
            return;
        }

        new_item /= 3; // Worry decreases since item not damaged.

        int target;
        if (new_item % monkeys[i].test_mod == 0) {
            target = monkeys[i].true_toss;
        }
        else {
            target = monkeys[i].false_toss;
        }

        monkeys[target].items.push_back(new_item);

        monkeys[i].inspect_count++;

        // std::cout << "  tosses " << new_item << " to " << target << '\n';
    }
}

int main() {
    std::vector<monkey> monkeys {};

    for (monkey monkey {}; std::cin >> monkey; ) {
        monkeys.push_back(monkey);
    }

    for (int i {0}; i < 20; i++) {
        for (int j {0}; j < static_cast<int>(monkeys.size()); j++) {
            process_monkey(monkeys, j);
        }
    }

    std::vector<int> inspections {};
    for (int i {0}; i < static_cast<int>(monkeys.size()); i++) {
        inspections.push_back(monkeys[i].inspect_count);
    }

    std::sort(inspections.begin(), inspections.end(), std::greater<int>());

    std::cout << inspections[0] * inspections[1] << '\n';
}
