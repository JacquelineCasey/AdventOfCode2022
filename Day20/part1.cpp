// Pulling out my hair because my code worked on the example and anything I could
// come up with, but not the input.
// It turns out the input has duplicates... 🫠

#include <iostream>
#include <vector>

/* https://stackoverflow.com/questions/12089514/real-modulo-operator-in-c-c */
// % is remaineder, so an adjustment is needed when negative numbers are used.
inline int modulo(int a, int b) {
    const int result = a % b;
    return result >= 0 ? result : result + b;
}

// Returns the index of item or -1 on failure.
int find(const std::vector<int>& vec, int item) {
    for (int i {0}; i < static_cast<int>(vec.size()); i++) {
        if (vec[i] == item) {
            return i;
        }
    }

    return -1;
}

// Returns the index of item or -1 on failure.
int find_item_with_number(const std::vector<std::pair<int, int>>& vec, int item_number) {
    for (int i {0}; i < static_cast<int>(vec.size()); i++) {
        if (vec[i].second == item_number) {
            return i;
        }
    }

    return -1;
}

// Note: this might not give the answer in the exact order the example gives, but
// it will be correct after some shift.
void move_item(std::vector<std::pair<int, int>>& vec, int item_number) {
    /* First, identify the index that contains this item. */
    int index {find_item_with_number(vec, item_number)};
    std::pair<int, int> entry {vec[index]};

    if (index == -1) {
        std::cout << "Something went wrong.\n";
        return;
    }

    int size {static_cast<int>(vec.size())};

    // Crossing the wrap around point does not count as a move, so we subtract 1 from size.
    int new_index {modulo(index + entry.first, size - 1)};

    /* Shift items around if necessary. */
    if (new_index > index) {
        for (int i {index}; i < new_index; i++) {
            vec[i] = vec[i+1];
        }
    }
    else if (new_index < index) {
        for (int i {index}; i > new_index; i--) {
            vec[i] = vec[i-1];
        }
    }

    vec[new_index] = entry;
}

std::vector<int> mix(const std::vector<int>& input) {
    std::vector<std::pair<int, int>> mixed {}; 

    // We tag each item with an 'item number' which states the order of movement.
    for (int i {0}; i < static_cast<int>(input.size()); i++) {
        mixed.push_back({input[i], i});
    }

    for (int i {0}; i < static_cast<int>(input.size()); i++) {
        move_item(mixed, i);
    }

    // remove item number info
    std::vector<int> result {};
    for (auto [item, _] : mixed) {
        result.push_back(item);
    }

    return result;
}


int main() {
    std::vector<int> input {};

    for (int item; std::cin >> item; ) {
        input.push_back(item);
    }

    std::vector<int> mixed {mix(input)};

    int zero_index {find(mixed, 0)};

    int size {static_cast<int>(mixed.size())};

    std::cout << mixed[(zero_index + 1000) % size] + mixed[(zero_index + 2000) % size] + mixed[(zero_index + 3000) % size] << '\n';
}
