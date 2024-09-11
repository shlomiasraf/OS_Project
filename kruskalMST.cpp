#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <stack>
#include <queue>
#include "Graph.hpp"
#include "kruskalMST.hpp"

// Utility function to find the parent of a node
int find(std::vector<int>& parent, int i) 
{
    if (parent[i] == i)
        return i;
    return parent[i] = find(parent, parent[i]);
}

// Function to perform union of two sets (by rank)
void Union(std::vector<int>& parent, std::vector<int>& rank, int x, int y) 
{
    int rootX = find(parent, x);
    int rootY = find(parent, y);

    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    } else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
}

// Utility function to perform DFS and return the farthest distance from the given node
std::pair<int, int> dfs(int node, const std::vector<std::vector<std::pair<int, int>>>& adj, std::vector<bool>& visited) 
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

// Function to find the longest distance (diameter) in the MST
int findLongestDistance(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
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

// Function to calculate the average distance between all vertex pairs in the MST
double calculateAverageDistance(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
{
    int totalDistance = 0;
    int pairCount = 0;

    for (int u = 0; u < V; ++u) {
        std::vector<int> dist(V, INT_MAX);
        std::queue<int> q;
        q.push(u);
        dist[u] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (const auto& [neighbor, weight] : adj[node]) {
                if (dist[neighbor] == INT_MAX) {
                    dist[neighbor] = dist[node] + weight;
                    q.push(neighbor);
                }
            }
        }

        for (int v = u + 1; v < V; ++v) {
            if (dist[v] != INT_MAX) {
                totalDistance += dist[v];
                pairCount++;
            }
        }
    }

    return (pairCount > 0) ? static_cast<double>(totalDistance) / pairCount : 0;
}

// Function to find the shortest distance between two distinct vertices in the MST
int findShortestDistance(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
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

// Function to print the MST and perform calculations
void printKruskalMST(const std::vector<std::vector<int>>& result, int V) 
{
    int weightMST = 0;
    std::vector<std::vector<std::pair<int, int>>> mstAdjList(V);

    std::cout << "Following are the edges in the constructed MST:\n";
    for (const auto& edge : result) 
    {
        std::cout << edge[0]+1 << " -- " << edge[1]+1 << " == " << edge[2] << "\n";
        weightMST += edge[2];

        mstAdjList[edge[0]].push_back({edge[1], edge[2]});
        mstAdjList[edge[1]].push_back({edge[0], edge[2]});
    }

    std::cout << "Total weight of the MST: " << weightMST << "\n";

    int longestDistance = findLongestDistance(mstAdjList, V);
    std::cout << "Longest distance between two vertices: " << longestDistance << "\n";

    double avgDistance = calculateAverageDistance(mstAdjList, V);
    std::cout << "Average distance between two vertices: " << avgDistance << "\n";

    int shortestDistance = findShortestDistance(mstAdjList, V);
    std::cout << "Shortest distance between two vertices in the MST: " << shortestDistance << "\n";
}

// Kruskal's algorithm for MST
void KruskalMST::kruskalfunction(Graph& graph) 
{
    int V = graph.V; 
    std::vector<std::vector<int>> edgeList;  // Store edges in {src, dest, weight} format

    // Step 1: Extract edges from the adjacency matrix
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            if (graph.adjList[u][v] != 0) {
                edgeList.push_back({u, v, graph.adjList[u][v]});
            }
        }
    }

    // Step 2: Sort edges based on weight
    std::sort(edgeList.begin(), edgeList.end(),
                [](const std::vector<int>& a, const std::vector<int>& b) {
                    return a[2] < b[2];
                });

    // Allocate memory for subsets (for Union-Find)
    std::vector<int> parent(V);
    std::vector<int> rank(V, 0);

    // Create V subsets (single elements)
    for (int v = 0; v < V; v++) {
        parent[v] = v;
    }

    std::vector<std::vector<int>> result;  // Store the resulting MST
    int e = 0;  // Number of edges in MST

    // Step 3: Pick the smallest edge and check if it forms a cycle
    for (const auto& edge : edgeList) {
        if (e >= V - 1) break; // Stop if MST has V-1 edges

        int u = edge[0];
        int v = edge[1];
        int weight = edge[2];

        int setU = find(parent, u);
        int setV = find(parent, v);

        if (setU != setV) {
            result.push_back({u, v, weight});
            e++;
            Union(parent, rank, setU, setV);
        }
    }

    // Print MST and perform calculations
    printKruskalMST(result, V);
}
