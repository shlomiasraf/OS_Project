#include "MSTInfo.hpp"

double MSTInfo::calculateTotalWeight(const std::vector<std::vector<std::pair<int, int>>> &adj, int V)
{
 int totalWeight = 0;
    std::vector<std::vector<std::pair<int, int>>> mstAdjList(V);
    // Iterate through each vertex and its adjacent vertices
    for (int u = 0; u < V; ++u) 
    {
        for (const auto& edge : adj[u]) 
        {
            int v = edge.first;  // Adjacent vertex
            int weight = edge.second;  // Weight of the edge
            // Ensure the edge is not counted twice by checking if v > u (only process each edge once)
            if (v > u) {
                totalWeight += weight;
        }
      }
    }
    return totalWeight;  // Return the total weight of the MST}
}

double MSTInfo::calculateShortestDistance(const std::vector<std::vector<std::pair<int, int>>> &adj, int V)
    {
    int shortestDist = INT_MAX;
    for (int u = 0; u < V; ++u) {
        for (const auto& [neighbor, weight] : adj[u]) {
            if (weight < shortestDist && u != neighbor) {
                shortestDist = weight;
            }
        }
    }

    return shortestDist;
    }
int MSTInfo::findLongestDistance(const std::vector<std::vector<std::pair<int, int>>> &adj, int V)
    {
       std::vector<bool> visited(V, false);

    // Perform the first DFS from an arbitrary node (let's pick node 0)
    auto [farthestNode, firstMaxDist] = dfs(0, adj, visited);

    // Reset visited array for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    // Perform the second DFS from the farthest node found in the first DFS
    auto [ignored, maxDistance] = dfs(farthestNode, adj, visited);

    // The maximum distance found is the diameter of the MST
    return maxDistance;
}

double MSTInfo::calculateAverageDistance(double totalweight, int V)
{
    if(V>0){
        return totalweight/V;
    }
    else return -1;
}

// Utility function to perform DFS and return the farthest distance from the given node
std::pair<int, int> MSTInfo::dfs(int node, const std::vector<std::vector<std::pair<int, int>>> &adj, std::vector<bool> &visited)
{
    std::stack<std::pair<int, int>> stack; // {node, distance}
    stack.push({node, 0});
    visited[node] = true;
    int farthestNode = node;
    int maxDistance = 0;

    while (!stack.empty()) {
        auto [currentNode, currentDist] = stack.top();
        stack.pop();

        if (currentDist > maxDistance) {
            maxDistance = currentDist;
            farthestNode = currentNode;
        }

        for (const auto& [neighbor, weight] : adj[currentNode]) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                stack.push({neighbor, currentDist + weight});
            }
        }
    }
    return {farthestNode, maxDistance};
}