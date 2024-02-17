#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::unordered_map<int, int> mp{{1,2},{12,4}};
    for (auto& [x, y] : mp) {
        std::cout << x << ' ' << y << std::endl;
    }
    std::vector<int> v{1,2,57,2,5};
    std::ranges::sort(v);
    for (auto x : v) std::cout << x << std::endl;
}