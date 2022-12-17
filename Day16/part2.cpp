/*
 * A cleaned up (and hopefully faster) version of part2.
 * 
 * The old version has too much wandering around between the agents. In this version,
 * they will decide which unopened valve to go to, and progress there without
 * distraction (alternatively, they may decide to halt).
 * 
 * First we will calculate a weighted graph that only contains nodes with a useful
 * valve, ie a valve with nonzero flow rate. 
 * 
 * Next, we calculate all the distances between nodes on the graph. This is done
 * with Floyd Warshall.
 * 
 * Finally, we run a similar (memoized) recursive algorithm. Each time an agent makes
 * a choice, it will elect to either move towards and activate a valve, or freeze.
 * If one agent is heading towards a valve, it is considered taken; for this reason,
 * we permit an agent to freeze even before all the valves are closed. Once both
 * agents freeze, we will stop.
 * 
 * Most of these ideas come from Borja. I was vaguely aware of the "make agents
 * move with purpose instead of wander aimlessly" idea, but using a smaller graph
 * with weighted edges did not occur to me at all.
 * 
 * Roughly 16s (with -O3) on my machine. I was expected a little faster honestly, 
 * memoizing (with a tree) took us from 55s to 22s, and switching to a hashtable 
 * only got us down to 17s. I discovered that hashtables are absolutely treacherous - 
 * if you screw up that hash function at all you are significantly slowed down vs 
 * the tree (4 mins).
 * 
 * I'm leaving out a few optimizations, for sure: the other version stored the set
 * as a long (we use a std::bitset, which probably holds a long, but has some extra
 * indirection probably).
 */

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>

class GraphNode {
public:
    int id;
    int flow_rate;
    std::vector<std::pair<std::reference_wrapper<GraphNode>, int>> neighbors {}; // weighted
    std::vector<int> distances {}; // Will hold the distance to every other node.

    GraphNode(int id, int flow_rate = 0) : id {id}, flow_rate {flow_rate}
    {}
};

class Graph {
public:
    // We need indirection, otherwise the reference wrappers we use start failing.
    std::vector<std::unique_ptr<GraphNode>> nodes {};

    void add_node(int flow_rate = 0) {
        int new_id {static_cast<int>(nodes.size())};
        nodes.push_back(std::make_unique<GraphNode>(new_id, flow_rate));
    }

    /* i and j must be in 0...nodes.size()-1. This edge is unidirectional, so call
     * this twice to get an undirected graph. No check is performed to gaurantee
     * that an edge is not added twice. */
    void add_edge(int i, int j, int weight = 1) {
        nodes[i]->neighbors.push_back({*nodes[j], weight});
    }

    /* Removes a node in the graph, and replaces it with edges between its neighbors
     * that represent the path through the removed node. If this results in duplicate
     * edges, the smaller of those edges is kept. Note that this effectively renumbers
     * all nodes after the removed node, so one should be careful when contracting 
     * repeatedly in a loop.
     * i must be in 0...nodes.size()-1. */
    void contract(int i) {
        /* Add edges between all neighbors. */
        for (auto [u_ref, u_weight] : nodes[i]->neighbors) {
            GraphNode& u {u_ref.get()};
            for (auto [v_ref, v_weight] : nodes[i]->neighbors) {
                GraphNode& v {v_ref.get()};
                if (u.id == v.id) {
                    continue;
                }

                /* Check if u and v are already neighbors. If so, update the edge if needed. */
                bool found {false};
                for (int j {0}; j < static_cast<int>(u.neighbors.size()); j++) {
                    auto [u_neighbor_ref, old_weight] = u.neighbors[j];
                    GraphNode& u_neighbor {u_neighbor_ref.get()};

                    if (u_neighbor.id == v.id) {
                        if (old_weight > u_weight + v_weight) {
                            u.neighbors[j].second = u_weight + v_weight;
                        }

                        found = true;
                        break;
                    }
                }

                /* Add new edge. */
                if (!found) {
                    u.neighbors.push_back({v, u_weight + v_weight});
                }
            }
        }

        /* Unlink the node from all neighbors. */
        for (auto& u : nodes) {
            // Iterate backwards!
            for (int j {static_cast<int>(u->neighbors.size()) - 1}; j >= 0; j--) {
                if (u->neighbors[j].first.get().id == i) {
                    u->neighbors.erase(u->neighbors.begin() + j);
                }
            }
        }

        /* Remove the node from the list. */
        nodes.erase(nodes.begin() + i);

        /* Finally, renumber all the nodes. */
        for (int j {0}; j < static_cast<int>(nodes.size()); j++) {
            nodes[j]->id = j;
        }
    }

    /* Initialized the distance vector inside every GraphNode, so that u.distances[v] 
     * is the distance from u to v. (u.distances[u] is 0). */
    void run_floyd_warshall() {
        for (auto& v : nodes) {
            v->distances = std::vector(nodes.size(), INT_MAX / 3); // Basically infinity, but no overflow.

            for (auto [u, weight] : v->neighbors) {
                v->distances[u.get().id] = weight;
            }

            v->distances[v->id] = 0;
        }

        int count {static_cast<int>(nodes.size())};
        for (int k {0}; k < count; k++) {
            for (int i {0}; i < count; i++) {
                for (int j {0}; j < count; j++) {
                    if (nodes[i]->distances[j] > nodes[i]->distances[k] + nodes[k]->distances[j]) {
                        nodes[i]->distances[j] = nodes[i]->distances[k] + nodes[k]->distances[j];
                    }
                }
            }
        }
    }

    void print() {
        for (auto& node : nodes) {
            for (auto& [neighbor, weight] : node->neighbors) {
                std::cout << node->id << " ---> " << neighbor.get().id << " (" << weight << ")\n";
            }
        }
    }
};

struct valve {
public:
    std::string name;
    int flow_rate;
    std::vector<std::string> neighbors; 

    friend std::istream& operator>>(std::istream& in, valve& valve) {
        valve = {}; // Clear valve.
        valve.neighbors = {}; // filled later.

        std::string buf;
        char ch;

        in >> buf >> valve.name;  // Valve {Name}
        in >> buf >> buf >> ch >> ch >> ch >> ch >> ch >> valve.flow_rate >> ch; // has flow rate={#};
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


struct agent_intent {
    int target; // Which valve (index) the agent is targeting. -1 if the agent rests.
    int time; // How much time will remain when the agent completes their action. Set to -1 negative number for rest.

    friend bool operator<(const agent_intent& lhs, const agent_intent& rhs) {
        if (lhs.time != rhs.time)
            return lhs.time < rhs.time;

        return lhs.target < rhs.target;
    }

    friend bool operator==(const agent_intent& lhs, const agent_intent& rhs) {
        return lhs.target == rhs.target && lhs.time == rhs.time;
    }
};

struct memo_key {
    unsigned long unopened; // We will unpack this before creating the key.
    agent_intent agent_1;
    agent_intent agent_2;

    friend bool operator<(const memo_key& lhs, const memo_key& rhs) {
        if (lhs.unopened != rhs.unopened)
            return lhs.unopened < rhs.unopened;
        if (lhs.agent_1 != rhs.agent_1)
            return lhs.agent_1 < rhs.agent_1;
        return lhs.agent_2 < rhs.agent_2;
    }

    friend bool operator==(const memo_key& lhs, const memo_key& rhs) {
        return lhs.unopened == rhs.unopened && lhs.agent_1 == rhs.agent_1 && lhs.agent_2 == rhs.agent_2;
    }
};

template<>
struct std::hash<memo_key> {
    std::uint64_t operator()(const memo_key& key) const {
        /* I don't claim to be all that good at hashing. This is mostly black magic. */
        return (key.unopened << 32) ^ (key.agent_1.time << 24) ^ (key.agent_1.target << 16)
            ^  (key.agent_2.time << 8) ^ (key.agent_1.target);
    }
};

/*
 * Returns the maximum pressure than can be released in the following situation:
 * The tunnel looks like `graph`.
 * The agents have intents `agent_1` and `agent_2`. They do not intend the open the
 * same valve (except possibly to encode the start condition), and agent_2 will
 * act no sooner than agent_1.
 * The unopened valves are known in `unopened`. AA is considered opened even though it
 * is not. No agent intends to open an opened valve, except for AA which encodes
 * the start condition.
 */
int max_pressure_release(const Graph& graph, 
        std::bitset<32>& unopened, 
        const agent_intent agent_1, 
        const agent_intent agent_2, 
        std::unordered_map<memo_key, int>& memo_map) {

    /* Base case - the agents ran out of time. */
    if (agent_1.time <= 0) {
        return 0;
    }

    /* If a memo lookup succeeds, just use that. */
    memo_key key {unopened.to_ulong(), agent_1, agent_2};
    auto it {memo_map.find(key)};
    if (it != memo_map.end()) {
        return it->second;
    }

    int pressure_released {0};

    // We won't let agent_1 rest under certain circumstances.
    bool can_rest {true};

    /* agent_1 acts now. They first consider all unopened valves. */
    for (int i {0}; i < static_cast<int>(graph.nodes.size()); i++) {
        if (!unopened[i])
            continue; 
        
        if (graph.nodes[agent_1.target]->distances[i] < agent_1.time) {
            /* This is feasible. */
            agent_intent next_agent_1 {.target = i, .time = agent_1.time - graph.nodes[agent_1.target]->distances[i] - 1}; // Travel and open
            agent_intent next_agent_2 {agent_2};

            /* These optimizations seem useful, but didn't actually do much. */
            if (agent_2.target == -1) {
                can_rest = false; // There is still work to do, and you partner is asleep.
            }
            else if (can_rest && graph.nodes[agent_2.target]->distances[i] + agent_2.time <= next_agent_1.time) {
                can_rest = false; // There is still *something* you can do faster than your partner. No rest.
            }

            // Pressure newly released by agent_1's action.
            int newly_released {graph.nodes[next_agent_1.target]->flow_rate * next_agent_1.time};

            if (next_agent_1 < next_agent_2) {
                std::swap(next_agent_1, next_agent_2);
            }

            unopened[i] = false;
            pressure_released = std::max(pressure_released, 
                max_pressure_release(graph, unopened, next_agent_1, next_agent_2, memo_map) + newly_released);
            unopened[i] = true;
        }
    }

    /* agent_1 also considers resting. */
    if (can_rest) {
        agent_intent next_agent_1 {.target = -1, .time = 0};
        agent_intent next_agent_2 {agent_2};

        if (next_agent_1 < next_agent_2) {
            std::swap(next_agent_1, next_agent_2);
        }

        pressure_released = std::max(pressure_released, max_pressure_release(graph, unopened, next_agent_1, next_agent_2, memo_map));
    }

    /* Cache for later use. */
    memo_map[key] = pressure_released;

    return pressure_released;
}


int main() {
    std::vector<valve> valves {};
    std::map<std::string, int> name_to_id {};
    Graph graph {};

    int AA_index {-1};

    /* Parse valves, add nodes. */
    int count = 0;
    for (valve valve; std::cin >> valve; ) {
        valves.push_back(valve);

        name_to_id[valve.name] = count;
        graph.add_node(valve.flow_rate);
        
        if (valve.name == "AA") {
            AA_index = count;
        }
        
        count++;
    }

    /* Add edges. */
    for (int i {0}; i < count; i++) {
        for (std::string neighbor : valves[i].neighbors) {
            graph.add_edge(i, name_to_id[neighbor]);
        }
    }

    /* Contract all vertices with 0 flow. This entails deleting nodes, so go backwards.
     * We also hold on to the AA vertex. */
    for (int i {static_cast<int>(graph.nodes.size()) - 1}; i >= 0; i--) {
        if (graph.nodes[i]->flow_rate == 0 && i != AA_index) {
            graph.contract(i);
        }
    }

    graph.run_floyd_warshall();

    std::bitset<32> unopened; // My inputs only needed around 16 bits.
    for (int i {0}; i < static_cast<int>(graph.nodes.size()); i++) {
        if (graph.nodes[i]->flow_rate == 0) {
            AA_index = i; // This needed to be updated after deletions.
            unopened[i] = false;
        }
        else {
            unopened[i] = true; // all nodes other than AA must be considered.
        }
    }

    agent_intent a1 {.target = AA_index, .time = 26};
    agent_intent a2 {.target = AA_index, .time = 26};

    std::unordered_map<memo_key, int> map {};
    std::cout << max_pressure_release(graph, unopened, a1, a2, map) << '\n';
}
