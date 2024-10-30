#ifndef MSTInfo_HPP
#define MSTInfo_HPP
#define INF INT_MAX

#include <limits.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
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
    static int find(std::vector<int>& parent, int i);
    static void Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y);
    static int minKey(const std::vector<int>& key, const std::vector<bool>& mstSet, int V);

};

#endif // MSTInfo_HPP
