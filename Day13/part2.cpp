

#include <iostream>
#include <sstream>
#include <vector>


struct value {
public:
    bool is_list;
    std::vector<value> inner_list {}; // I'm a litle surprised this is allowed.
    int inner_int;

    bool divider = false;

    friend bool operator<(value left, value right);
};

value from_sstream(std::stringstream& sstream) {
    value val {};
    if (sstream.peek() == '[') {
        val.is_list = true;
        val.inner_list = {};

        char ch;
        sstream >> ch;

        bool remove_comma = false;
        while (sstream.peek() != ']') {
            if (remove_comma) {
                sstream >> ch;
            }
            value inner_val {from_sstream(sstream)};
            val.inner_list.push_back(inner_val);
            remove_comma = true;
        }

        sstream >> ch;
    }
    else {
        val.is_list = false;
        sstream >> val.inner_int;
    }

    return val;
}

// -1 for wrong order, +1 for right order, 0 for unknown / keep going.
int right_order(value left, value right) {
    if (!left.is_list && !right.is_list) {
        if (left.inner_int < right.inner_int) {
            return 1;
        }
        else if (left.inner_int > right.inner_int) {
            return -1;
        }
        else {
            return 0;
        }
    }
    else if (left.is_list && right.is_list) {
        int i {0};
        while (i < static_cast<int>(left.inner_list.size()) && i < static_cast<int>(right.inner_list.size())) {
            int comparision = right_order(left.inner_list[i], right.inner_list[i]);
            if (comparision != 0) {
                return comparision;
            }

            i++;
        }

        // Check which ran out...
        if (left.inner_list.size() < right.inner_list.size()) {
            return 1;
        }
        else if (left.inner_list.size() > right.inner_list.size()) {
            return -1;
        }
        else {
            return 0;
        }
    }
    else { // one and only one is an int
        if (!left.is_list) {
            value inner_val {};
            inner_val.is_list = false;
            inner_val.inner_int = left.inner_int;

            left.is_list = true;
            left.inner_list = {inner_val};
            return right_order(left, right);
        }
        else {
            value inner_val {};
            inner_val.is_list = false;
            inner_val.inner_int = right.inner_int;

            right.is_list = true;
            right.inner_list = {inner_val};
            return right_order(left, right);
        }
    }
}

bool operator<(value left, value right) {
    return right_order(left, right) == 1;
}


int main() {
    std::vector<value> values {};
    std::string line_1;
    std::string line_2;
    while (std::cin >> line_1 >> line_2) {
        std::stringstream left_stream {std::stringstream(line_1)};
        value left {from_sstream(left_stream)};
        std::stringstream right_stream {std::stringstream(line_2)};
        value right {from_sstream(right_stream)};

        values.push_back(left);
        values.push_back(right);
    }
    
    std::stringstream sstream {"[[2]]"};
    values.push_back(from_sstream(sstream));
    values.back().divider = true;

    sstream = std::stringstream{"[[6]]"};
    values.push_back(from_sstream(sstream));
    values.back().divider = true;

    std::sort(values.begin(), values.end());

    int product {1};
    for (int i {0}; i < static_cast<int>(values.size()); i++) {
        if (values[i].divider) {
            product *= i + 1; // 1 based index;
        }
    }

    std::cout << product << '\n';
}
