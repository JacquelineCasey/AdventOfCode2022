// The slower original version improved by part2.cpp
// Commence bit bashing. There are < 64 valves... coincidence? 

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

struct valve {
public:
    int id;
    std::string name_str; // only used for parsing
    int rate;

    std::vector<int> neighbors;
    std::vector<std::string> neighbors_str; // only used for parsing

    friend std::istream& operator>>(std::istream& in, valve& valve) {
        valve = {}; // Clear valve.
        valve.neighbors_str = {};
        valve.neighbors = {}; // filled later.

        std::string buf;
        char ch;

        in >> buf >> valve.name_str;  // Valve {Name}
        in >> buf >> buf >> ch >> ch >> ch >> ch >> ch >> valve.rate >> ch; // has flow rate={#};
        in >> buf >> buf >> buf >> buf; // tunnels lead to valves 

        // Parse the list of neighbors.
        in >> buf;
        while (buf.size() == 3) { // while buf contains a comma
            valve.neighbors_str.push_back(buf.substr(0, 2));
            in >> buf;
        }
        valve.neighbors_str.push_back(buf); // last one.

        return in;
    }
};

// Memoization is a good idea.
struct memo_table_key {
    std::uint64_t opened_valves;
    int from_human;
    int from_elphant;
    int time_left;

    friend bool operator==(const memo_table_key& lhs, const memo_table_key& rhs) {
        return lhs.time_left == rhs.time_left 
            && lhs.from_human == rhs.from_human
            && lhs.from_elphant == rhs.from_elphant
            && lhs.opened_valves == rhs.opened_valves;
    }
};

template<>
struct std::hash<std::set<std::string>> {
    long operator()(const std::set<int>& set) const {
        long long hash {};
        for (int item : set) { // We use that the items are fixed length.
            hash = (hash * 3) << 1;
            hash ^= item;
        }

        return hash;
    }
};

// I was overthinking this, there is no reason the do this...
// void init_hash(memo_table_key& key) {
//     key.hash = (std::hash<std::string>{}(key.from_human + key.from_elphant) << 5) ^ (std::hash<int>{}(key.time_left));
//     key.hash ^= (std::hash<std::set<std::string>>{}(key.opened_valves)) << 10;
// }

template<>
struct std::hash<memo_table_key> {
    // I don't know how to make a good hash function :(
    std::uint64_t operator()(const memo_table_key& key) const {
        std::uint64_t hash = ((key.from_human * 64 + key.from_elphant) << 8) ^ ((key.time_left) << 16);
        hash ^= key.opened_valves;
        return hash;
    }
};


// Returns how well you can do, disregarding the pressure released by already opened
// valves (but considering which valves have been opened).
int max_pressure_release(const std::vector<valve>& valves, 
        std::uint64_t opened_mask,
        std::unordered_map<memo_table_key, int>& memo_table, 
        int from_human, 
        int from_elephant,
        int time_left,
        std::uint64_t useful_valves
    ) {

    /* Base case - you can't gain any benefit in 1 minute */
    if (time_left == 1) {
        return 0;
    }

    /* New base case - you can't gain any benefit after opening all valves. */
    if (opened_mask == useful_valves) { 
        // std::cout << "All valves opened!\n";
        return 0;
    }

    /* Attempt a look up in memo table; */

    memo_table_key key;
    key.opened_valves = opened_mask;
    key.from_human = from_human;
    key.from_elphant = from_elephant;
    key.time_left = time_left;

    if (memo_table.contains(key)) {
        return memo_table[key];
    }

    /* Figure out what is best between opening the current valve and walking somewhere else. */
    int max_release {0};
    const valve& human_valve {valves[from_human]};
    const valve& elephant_valve {valves[from_elephant]};

    /* Consider choices for both human and elephant */
    std::vector<int> human_choices = human_valve.neighbors;
    std::uint64_t h_mask {1};
    h_mask <<= from_human;
    if (human_valve.rate > 0 && (opened_mask & h_mask) == 0) {
        human_choices.push_back(from_human); // This will represent opening the valve.
    }

    std::vector<int> elephant_choices = elephant_valve.neighbors;
    std::uint64_t e_mask {1};
    e_mask <<= from_elephant;
    if (elephant_valve.rate > 0 && (opened_mask & e_mask) == 0) {
        elephant_choices.push_back(from_elephant); // This will represent opening the valve.
    }

    for (int human_choice : human_choices) {
        for (int elephant_choice : elephant_choices) {
            if (from_elephant == from_human && from_elephant == elephant_choice && from_human == human_choice) {
                // They can't both open the same valve.
                continue;
            }

            std::uint64_t new_opened_mask = opened_mask;

            int elephant_benefit {0};
            int human_benefit {0};
            if (from_elephant == elephant_choice) {
                new_opened_mask |= e_mask;
                elephant_benefit = (time_left - 1) * elephant_valve.rate;
            }

            if (from_human == human_choice) {
                new_opened_mask |= h_mask;
                human_benefit = (time_left - 1) * human_valve.rate;
            }

            max_release = std::max(max_release, 
                max_pressure_release(valves, new_opened_mask, memo_table, human_choice, elephant_choice, time_left-1, useful_valves)
                + human_benefit + elephant_benefit);
        }
    }

    /* Store the result in the memo table */
    memo_table[key] = max_release;

    return max_release;
}

int main() {
    std::vector<valve> valves {};
    std::map<std::string, int> code_to_int {};

    int count = 0;

    std::uint64_t useful_valves {0};
    std::uint64_t single_bit_mask {1};
    for (valve valve; std::cin >> valve; ) {
        valves.push_back(valve);
        valve.id = count;
        if (valve.rate > 0) {
            useful_valves |= (single_bit_mask << count);
        }

        code_to_int[valve.name_str] = count;
        count++;
    }

    // Fill in ints for valve codes.
    for (valve& valve : valves) {
        for (std::string str : valve.neighbors_str) {
            valve.neighbors.push_back(code_to_int[str]);
        }
    }

    int start_id {code_to_int["AA"]};

    std::unordered_map<memo_table_key, int> memo_table;
    std::uint64_t opened_mask {0};
    int result {max_pressure_release(valves, opened_mask, memo_table, start_id, start_id, 10, useful_valves)};

    std::cout << result << '\n';
}
