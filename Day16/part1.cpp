// First, I took a second to convince myself this is at least close to NP, then
// I coded up a recursive algorithm with memoization. This solved the example,
// but took too long for the input. I then spent a large amount of time trying
// to optimize my way out of that (I switched to hashing, which is more efficient
// but more of a pain. I sat stumped a while after that, reread the prompt, saw
// the cheeky line "its flow rate is 0, so there's no point in opening it", and
// added an optimazation where you no longer consider opening a 0 flow valve.
// This was all that I needed. This runs in 2.5 seoconds with -O3 optimizations.

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

struct valve {
public:
    std::string name;
    int rate;

    std::vector<std::string> neighbors;

    friend bool operator<(const valve& lhs, const valve& rhs) {
        return lhs.name < rhs.name; // we use the fact that the names are unique.
    }

    friend std::istream& operator>>(std::istream& in, valve& valve) {
        valve = {}; // Clear valve.
        valve.neighbors = {};

        std::string buf;
        char ch;

        in >> buf >> valve.name;  // Valve {Name}
        in >> buf >> buf >> ch >> ch >> ch >> ch >> ch >> valve.rate >> ch; // has flow rate={#};
        in >> buf >> buf >> buf >> buf; // tunnels lead to valves 

        // Parse the list of neighbors.
        in >> buf;
        while (buf.size() == 3) { // while buf contains a comma
            valve.neighbors.push_back(buf.substr(0, 2));
            in >> buf;
        }
        valve.neighbors.push_back(buf); // last one.

        return in;
    }
};

// Memoization is a good idea.
struct memo_table_key {
    std::set<std::string> opened_valves {};
    std::string from;
    int time_left;

    long hash;

    friend bool operator==(const memo_table_key& lhs, const memo_table_key& rhs) {
        return lhs.hash == rhs.hash 
            && lhs.time_left == rhs.time_left 
            && lhs.from == rhs.from
            && lhs.opened_valves == rhs.opened_valves;
    }

    // // might switch to hashtable later...
    // friend bool operator<(const memo_table_key& lhs, const memo_table_key& rhs) {
    //     if (lhs.time_left != rhs.time_left) {
    //         return lhs.time_left < rhs.time_left;
    //     }
    //     if (lhs.from != rhs.from) {
    //         return lhs.from < rhs.from;
    //     }
    //     if (lhs.opened_valves.size() != rhs.opened_valves.size()) {
    //         return lhs.opened_valves.size() < rhs.opened_valves.size();
    //     }
    //     return lhs.opened_valves < rhs.opened_valves; // lexicographic comparison.
    // }

};

template<>
struct std::hash<std::set<std::string>> {
    long operator()(const std::set<std::string>& set) const {
        std::string s {};
        for (std::string item : set) { // We use that the items are fixed length.
            s += item;
        }

        return std::hash<std::string>{}(s);
    }
};

void init_hash(memo_table_key& key) {
    key.hash = (std::hash<std::string>{}(key.from) << 5) ^ (std::hash<int>{}(key.time_left));
    key.hash ^= (std::hash<std::set<std::string>>{}(key.opened_valves)) << 10;
}

template<>
struct std::hash<memo_table_key> {
    long operator()(const memo_table_key& key) const {
        return key.hash;
    }
};


// Returns how well you can do, disregarding the pressure released by already opened
// valves (but considering which valves have been opened).
int max_pressure_release(const std::map<std::string, valve>& valves, 
        std::set<std::string>& opened, 
        std::unordered_map<memo_table_key, int>& memo_table, 
        std::string from, 
        int time_left
    ) {

    /* Base case - you can't gain any benefit in 1 minute */
    if (time_left == 1) {
        return 0;
    }

    /* Attempt a look up in memo table; */

    memo_table_key key;
    key.opened_valves = opened;
    key.from = from;
    key.time_left = time_left;
    init_hash(key);

    if (memo_table.contains(key)) {
        return memo_table.at(key);
    }

    /* Figure out what is best between opening the current valve and walking somewhere else. */
    int max_release {0};
    const valve& curr_valve {valves.at(from)};

    // Adding the curr_valve.rate > 0 here is the change that made this work in time... 
    // I don't know if all the hashing was actually necessary.
    if (curr_valve.rate > 0 && !opened.contains(from)) { 
        /* Try opening the valve. */

        opened.insert(from);
        int release {max_pressure_release(valves, opened, memo_table, from, time_left-1)};
        release += (time_left - 1) * curr_valve.rate; // benefit from open.
        max_release = std::max(max_release, release);
        opened.erase(from);
    }

    for (std::string next : curr_valve.neighbors) {
        /* Try going to that valve. */

        int release {max_pressure_release(valves, opened, memo_table, next, time_left-1)};
        max_release = std::max(max_release, release);
    }

    /* Store the result in the memo table */
    memo_table[key] = max_release;

    return max_release;
}

int main() {
    std::map<std::string, valve> valves {};

    for (valve valve; std::cin >> valve; ) {
        valves[valve.name] = valve;
    }

    // We start at AA regardless of where it is in the input.

    // It seems like an NP problem, but 30 is small enough that it might be fine
    // to attempt via memoization.

    std::unordered_map<memo_table_key, int> memo_table;
    std::set<std::string> opened {};
    int result {max_pressure_release(valves, opened, memo_table, "AA", 30)};

    std::cout << result << '\n';
}
