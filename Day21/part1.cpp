
#include <iostream>
#include <map>

enum class Operation {
    PLUS,
    MINUS,
    TIMES,
    DIVIDE
};

/* A thunk is an unevaluated value. I'm thinking Haskell style lazy evaluation here. */
struct Thunk {
    std::string left_id;
    std::string right_id;
    Operation op;
};

struct Monkey {
    std::string id;

    // When a thunk monkey is evaluated, it will switch to an int monkey.
    std::variant<long long, Thunk> number;

    long long evaluate(std::map<std::string, Monkey>& monkeys) {
        if (std::holds_alternative<long long>(number)) {
            return std::get<long long>(number);
        }
        
        Thunk thunk = std::get<Thunk>(number);
        long long left {monkeys[thunk.left_id].evaluate(monkeys)};
        long long right {monkeys[thunk.right_id].evaluate(monkeys)};

        long long result;
        switch (thunk.op) {
        case Operation::PLUS:
            result = left + right;
            break;
        case Operation::MINUS:
            result = left - right;
            break;
        case Operation::TIMES:
            result = left * right;
            break;
        case Operation::DIVIDE:
            result = left / right;
            break;
        default:
            std::cout << "Something went wrong\n";
            result = -42;
        }

        number = result; // The thunk has been evaluated. Save the result.
        return result;
    }

    friend std::istream& operator>>(std::istream& in, Monkey& monkey) {
        monkey = {};
        in >> monkey.id;
        monkey.id = monkey.id.substr(0, 4); // Remove the colon.

        std::string buf;
        in >> buf;

        if (isdigit(buf[0])) {
            // int monkey.
            monkey.number = std::stoi(buf);
        }
        else {
            // thunk monkey.
            Thunk thunk;
            thunk.left_id = buf;
            char ch {};
            in >> ch;

            switch (ch) {
            case '+':
                thunk.op = Operation::PLUS;
                break;
            case '-':
                thunk.op = Operation::MINUS;
                break;
            case '*':
                thunk.op = Operation::TIMES;
                break;
            case '/':
                thunk.op = Operation::DIVIDE;
                break;
            }

            in >> thunk.right_id;
            monkey.number = thunk;
        }

        return in;
    }
};


int main() {
    std::map<std::string, Monkey> monkeys {};

    for (Monkey monkey; std::cin >> monkey; ) {
        monkeys[monkey.id] = monkey;
    }

    std::cout << monkeys["root"].evaluate(monkeys) << '\n';
}
