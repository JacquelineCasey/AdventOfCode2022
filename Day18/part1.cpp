
#include <array>
#include <iostream>
#include <set>


using vec3 = std::tuple<int, int, int>;

std::istream& operator>>(std::istream& in, vec3& vec) {
    char ch;
    auto& [x, y, z] {vec};
    return in >> x >> ch >> y >> ch >> z;
}

constexpr std::array<vec3, 6> directions {{
    {-1,  0,  0},
    { 1,  0,  0},
    { 0, -1,  0},
    { 0,  1,  0},
    { 0,  0, -1},
    { 0,  0,  1}
}};

int main() {
    std::set<vec3> cubes {};
    for (vec3 cube; std::cin >> cube; ) {
        cubes.insert(cube);
    }

    int surface_area {0};
    for (const vec3& cube : cubes) {
        auto [x, y, z] {cube};
        for (const vec3& dir : directions) {
            auto [dx, dy, dz] {dir};

            if (!cubes.contains({x + dx, y + dy, z + dz})) {
                surface_area++;
            }
        }
    }

    std::cout << surface_area << '\n';
}
