// Exciting! I think I like this one a lot.

#include <iostream>
#include <map>
#include <numeric>
#include <vector>

enum class Operation {
    PLUS,
    MINUS,
    TIMES,
    DIVIDE
};

// An expression kept in some kind of normal form:
// (ax)/b + c, where a b and c are integers and x is the number we get to choose as humn.
struct expression {
    long long a {0};
    long long b {1};
    long long c {0};

    // Given two expressions, solves for x.
    friend long long solve(expression left, expression right) {
        // Put x's on the left and constants on the right.
        expression left_tmp {left - right};
        left_tmp.c = 0;
        right.c -= left.c;
        left = left_tmp;

        // Multiply by the denominator.
        right.c *= left.b;
        left.b = 1;

        // Divide by the numerator coefficent
        right.c /= left.a;

        // We now have x = {constant}
        return right.c;
    }

    expression reduce() {
        long long gcd {std::gcd(a, b)};

        return expression {
            .a = a / gcd,
            .b = b / gcd,
            .c = c
        };
    }

    /* We define the operators for ease of use. */
    friend expression operator+(const expression& lhs, const expression& rhs) {
        long long lcm {std::lcm(lhs.b, rhs.b)};

        return expression {
            .a = lhs.a * (lcm / lhs.b) + rhs.a * (lcm / rhs.b),
            .b = lcm,
            .c = lhs.c + rhs.c
        }.reduce();
    }

    friend expression operator-(const expression& lhs, const expression& rhs) {
        // Defer to operator+
        return lhs + expression {.a = -rhs.a, .b = rhs.b, .c = -rhs.c};
    }

    friend expression operator*(const expression& lhs, const expression& rhs) {
        // I strongly suspect that only one operand will ever contain x. Let's only do that.
        if (lhs.a != 0 && rhs.a != 0) {
            std::cout << "Warning, both terms contain x...\n";
            return {};
        }

        // Let's put the x term on the left.
        if (rhs.a != 0) {
            return rhs * lhs;
        }

        return expression {
            .a = lhs.a * rhs.c,
            .b = lhs.b,
            .c = lhs.c * rhs.c
        }.reduce();
    }

    // This one might be the hardest to get right.
    friend expression operator/(const expression& lhs, const expression& rhs) {
        // Again, we assume only one side has x. Actually, we will assume that 
        // rhs doesn't have x at all.
        if (rhs.a != 0) {
            std::cout << "Warning, right side contains x...\n";
            return {};
        }

        return expression {
            .a = lhs.a,
            .b = lhs.b * rhs.c,
            .c = lhs.c / rhs.c
        }.reduce();
    }
};

/* A thunk is an unevaluated value. I'm thinking Haskell style lazy evaluation here. */
struct Thunk {
    std::string left_id;
    std::string right_id;
    Operation op;
};

struct Monkey {
    std::string id;

    // When a thunk monkey is evaluated, it will switch to an *expression* monkey.
    std::variant<std::vector<expression>, Thunk> expr;

    // This returns a vector. Only 'root' will return 2 elements though.
    std::vector<expression> evaluate(std::map<std::string, Monkey>& monkeys) {
        if (std::holds_alternative<std::vector<expression>>(expr)) {
            return std::get<std::vector<expression>>(expr);
        }
        
        Thunk thunk = std::get<Thunk>(expr);
        expression left {monkeys[thunk.left_id].evaluate(monkeys)[0]};
        expression right {monkeys[thunk.right_id].evaluate(monkeys)[0]};

        // Root is a special case - returns both sides.
        if (id == "root") {
            expr = std::vector {left, right};
            return std::vector {left, right};
        }

        expression result;
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
            result = {};
        }

        expr = std::vector {result}; // The thunk has been evaluated. Save the result.
        return std::vector {result};
    }

    friend std::istream& operator>>(std::istream& in, Monkey& monkey) {
        monkey = {};
        in >> monkey.id;
        monkey.id = monkey.id.substr(0, 4); // Remove the colon.

        std::string buf;
        in >> buf;

        if (isdigit(buf[0])) {
            // int monkey.
            expression expr {};
            expr.c = std::stoi(buf);
            monkey.expr = std::vector{expr};
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
            monkey.expr = thunk;
        }

        // Override expression of humn
        if (monkey.id == "humn") {
            monkey.expr = std::vector {
                expression {.a = 1, .b = 1, .c = 0}
            };
        }

        return in;
    }
};


int main() {
    std::map<std::string, Monkey> monkeys {};

    for (Monkey monkey; std::cin >> monkey; ) {
        monkeys[monkey.id] = monkey;
    }

    std::vector<expression> expr {monkeys["root"].evaluate(monkeys)};

    std::cout << solve(expr[0], expr[1]) << '\n';
}
