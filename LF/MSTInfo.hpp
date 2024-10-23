#ifndef MSTInfo_HPP
#define MSTInfo_HPP

#include <vector>
#include <climits>
#include <utility> // For std::pair
#include <stack>
#include "Graph.hpp"
class MSTInfo {
public:
    static double calculateTotalWeight(const std::vector<std::vector<std::pair<int, int>>>& adj, int V);
    static double calculateShortestDistance(const std::vector<std::vector<std::pair<int, int>>>& adj, int V);
    static int findLongestDistance(const std::vector<std::vector<std::pair<int, int>>>& adj, int V);
    static double calculateAverageDistance(double totalweight, int V);
    static std::pair<int, int> dfs(int node, const std::vector<std::vector<std::pair<int, int>>>& adj, std::vector<bool>& visited);

};

#endif // MSTInfo_HPP
