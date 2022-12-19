
#include <iostream>
#include <unordered_map>
#include <vector>

/* https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x */
inline void hash_combine([[maybe_unused]] std::size_t& seed) { }

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    hash_combine(seed, rest...);
}


struct memo_key {
    int ore_per_second;
    int clay_per_second;
    int obsidian_per_second;
    int geodes_per_second;
    int ore;
    int clay;
    int obsidian;

    int time_left; 

    /* Lexicographic compare items. Also gives relational operators. C++20. */
    friend auto operator<=>(const memo_key&, const memo_key&) = default;
};

template<>
struct std::hash<memo_key> {
    std::size_t operator()(const memo_key& key) const {
        std::size_t result {0};
        hash_combine(result, 
            key.ore_per_second, 
            key.clay_per_second, 
            key.obsidian_per_second, 
            key.geodes_per_second,
            key.ore,
            key.clay,
            key.obsidian,
            key.time_left
        );
        return result;
    }
};


struct blueprint {
    int id;
    int ore_robot_ore_cost;
    int clay_robot_ore_cost;
    int obsidian_robot_ore_cost;
    int obsidian_robot_clay_cost;
    int geode_robot_ore_cost;
    int geode_robot_obsidian_cost;

    std::unordered_map<memo_key, int> memo_map; // Maps max_geode_production calls to geode number.

    friend std::istream& operator>>(std::istream& in, blueprint& blueprint) {
        std::string buf;
        char ch;

        // Blueprint #:
        in >> buf >> blueprint.id >> ch; 
        // Each ore robot costs # ore.
        in >> buf >> buf >> buf >> buf >> blueprint.ore_robot_ore_cost >> buf; 
        // Each clay robot costs # ore.
        in >> buf >> buf >> buf >> buf >> blueprint.clay_robot_ore_cost >> buf; 
        // Each obsidian robot costs # ore and # clay.
        in >> buf >> buf >> buf >> buf >> blueprint.obsidian_robot_ore_cost >> buf >> buf >> blueprint.obsidian_robot_clay_cost >> buf; 
        // Each geode robot costs # ore and # obsidian.
        in >> buf >> buf >> buf >> buf >> blueprint.geode_robot_ore_cost >> buf >> buf >> blueprint.geode_robot_obsidian_cost >> buf;

        return in;
    };

    void add_one_minute_yield(memo_key& query) {
        query.ore += query.ore_per_second;
        query.clay += query.clay_per_second;
        query.obsidian += query.obsidian_per_second;

        query.time_left--;
    }

    int max_geode_production(const memo_key& query) {
        if (query.time_left == 0) {
            return 0;
        }

        if (memo_map.contains(query)) {
            return memo_map[query];
        }

        /* We consider trying to construct all 4 robots. */
        int max_production {0};
        
        /* Ore robot. */
        memo_key next_query {query};
        int production {0};
        while (next_query.ore < ore_robot_ore_cost) {
            add_one_minute_yield(next_query);
            production += query.geodes_per_second;
        }
        if (next_query.time_left > 0) {
            add_one_minute_yield(next_query);
            production += query.geodes_per_second;
            next_query.ore_per_second++;
            next_query.ore -= ore_robot_ore_cost;
            production += max_geode_production(next_query);
            max_production = std::max(max_production, production);
        }

        /* Clay robot. */
        next_query = query;
        production = 0;
        while (next_query.ore < clay_robot_ore_cost) {
            add_one_minute_yield(next_query);
            production += query.geodes_per_second;
        }
        if (next_query.time_left > 0) {
            add_one_minute_yield(next_query);
            production += query.geodes_per_second;
            next_query.clay_per_second++;
            next_query.ore -= clay_robot_ore_cost;
            production += max_geode_production(next_query);
            max_production = std::max(max_production, production);
        }

        /* Obsidian robot. */
        next_query = query;
        production = 0;
        if (query.clay_per_second > 0) {
            while (next_query.ore < obsidian_robot_ore_cost || next_query.clay < obsidian_robot_clay_cost) {
                add_one_minute_yield(next_query);
                production += query.geodes_per_second;
            }
            if (next_query.time_left > 0) {
                add_one_minute_yield(next_query);
                production += query.geodes_per_second;
                next_query.obsidian_per_second++;
                next_query.ore -= obsidian_robot_ore_cost;
                next_query.clay -= obsidian_robot_clay_cost;
                production += max_geode_production(next_query);
                max_production = std::max(max_production, production);
            }
        }

        /* Geode robot. */
        next_query = query;
        production = 0;
        if (query.obsidian_per_second > 0) {
            while (next_query.ore < geode_robot_ore_cost || next_query.obsidian < geode_robot_obsidian_cost) {
                add_one_minute_yield(next_query);
                production += query.geodes_per_second;
            }
            if (next_query.time_left > 0) {
                add_one_minute_yield(next_query);
                production += query.geodes_per_second;
                next_query.geodes_per_second++;
                next_query.ore -= geode_robot_ore_cost;
                next_query.obsidian -= geode_robot_obsidian_cost;
                production += max_geode_production(next_query);
                max_production = std::max(max_production, production);
            }
        }

        /* Or (at the end), consider doing nothing. */
        if (max_production == 0) {
            max_production = query.time_left * query.geodes_per_second;
        }

        memo_map[query] = max_production;
        return max_production;
    }
};


int main() {
    std::vector<blueprint> blueprints {};
    for (blueprint blueprint; std::cin >> blueprint; ) {
        blueprints.push_back(blueprint);
    }

    int quality_sum {0};
    for (blueprint& blueprint : blueprints) {
        memo_key query {
            .ore_per_second = 1, 
            .clay_per_second = 0, 
            .obsidian_per_second = 0, 
            .geodes_per_second = 0,
            .ore = 0, 
            .clay = 0, 
            .obsidian = 0, 
            .time_left = 24
        };
        int production {blueprint.max_geode_production(query)};
        quality_sum += blueprint.id * production;
        // std::cout << blueprint.id << " had max production " << production << '\n';
    }

    std::cout << quality_sum << '\n';
}
