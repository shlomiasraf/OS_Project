#include <iostream>
#include <vector>
#include <limits.h>
#include <stack>
#include <queue>
#include "Graph.hpp"
#include "primMST.hpp"

#define INF INT_MAX

// Function to find the vertex with the minimum key value
int minKey(const std::vector<int>& key, const std::vector<bool>& mstSet, int V) {
    int min = INF, min_index = -1;

    for (int v = 0; v < V; v++) {
        if (!mstSet[v] && key[v] < min) {
            min = key[v];
            min_index = v;
        }
    }
    return min_index;
}

// DFS utility for calculating longest distance
std::pair<int, int> dfsPrim(int node, const std::vector<std::vector<std::pair<int, int>>>& adj, std::vector<bool>& visited) 
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

// Function to calculate the longest distance in the MST (diameter)
int findLongestDistancePrim(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
{
    std::vector<bool> visited(V, false);

    // Perform DFS from an arbitrary node (starting with node 0)
    auto [farthestNode, _] = dfsPrim(0, adj, visited);

    // Reset visited array for the second DFS
    std::fill(visited.begin(), visited.end(), false);

    // Perform second DFS from the farthest node found in the first DFS
    auto [ignored, maxDistance] = dfsPrim(farthestNode, adj, visited);

    return maxDistance;
}

// Function to calculate the average distance between all vertex pairs in the MST
double calculateAverageDistancePrim(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
{
    int totalDistance = 0;
    int pairCount = 0;

    for (int u = 0; u < V; ++u) {
        std::vector<int> dist(V, INF);
        std::queue<int> q;
        q.push(u);
        dist[u] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (const auto& [neighbor, weight] : adj[node]) {
                if (dist[neighbor] == INF) {
                    dist[neighbor] = dist[node] + weight;
                    q.push(neighbor);
                }
            }
        }

        for (int v = u + 1; v < V; ++v) {
            if (dist[v] != INF) {
                totalDistance += dist[v];
                pairCount++;
            }
        }
    }

    return (pairCount > 0) ? static_cast<double>(totalDistance) / pairCount : 0;
}

// Function to find the shortest distance between two distinct vertices in the MST
int findShortestDistancePrim(const std::vector<std::vector<std::pair<int, int>>>& adj, int V) 
{
    int shortestDist = INF;

    for (int u = 0; u < V; ++u) {
        std::vector<int> dist(V, INF);
        std::queue<int> q;
        q.push(u);
        dist[u] = 0;

        while (!q.empty()) {
            int node = q.front();
            q.pop();

            for (const auto& [neighbor, weight] : adj[node]) {
                if (dist[neighbor] == INF) {
                    dist[neighbor] = dist[node] + weight;
                    q.push(neighbor);

                    // Check if this distance is the shortest found so far
                    if (dist[neighbor] < shortestDist && neighbor != u) {
                        shortestDist = dist[neighbor];
                    }
                }
            }
        }
    }

    return (shortestDist < INF) ? shortestDist : -1; // Return -1 if no path found
}

// Function to print the constructed MST and calculate required properties
void printMST(const std::vector<int>& parent, const Graph& graph) 
{
    int weightMST = 0;
    int V = graph.V;
    std::vector<std::vector<std::pair<int, int>>> adj(V); // Adjacency list for MST (for calculating distances)
    std::cout << "Edge \tWeight\n";
    for (int i = 1; i < V; i++) 
    {
        int u = parent[i];
        int v = i;
        int weight = graph.adjList[u][v]; // Access the weight of the edge from the original graph
        adj[u].push_back({v, weight});
        adj[v].push_back({u, weight}); // Add the edge to the adjacency list
        std::cout << u + 1 << " - " << v + 1 << " \t" << weight << "\n";
        weightMST += weight;
    }
    std::cout << "Total weight of the MST: " << weightMST << "\n";

    // Longest distance
    int longestDistance = findLongestDistancePrim(adj, V);
    std::cout << "Longest distance between two vertices in MST: " << longestDistance << "\n";

    // Average distance between vertices in MST
    double avgDistance = calculateAverageDistancePrim(adj, V);
    std::cout << "Average distance between two vertices in the MST: " << avgDistance << "\n";

    // Shortest distance between two distinct vertices in the MST
    int shortestDistance = findShortestDistancePrim(adj, V);
    std::cout << "Shortest distance between two vertices in the MST: " << shortestDistance << "\n";
}

// Prim's algorithm for constructing the MST
void PrimMST::primFunction(Graph& graph) 
{
    int V = graph.V;
    std::vector<int> parent(V);  // Array to store the constructed MST
    std::vector<int> key(V, INF); // Key values used to pick minimum weight edge
    std::vector<bool> mstSet(V, false); // To represent the set of vertices included in MST

    key[0] = 0; // Start with the first vertex
    parent[0] = -1; // The first node is always the root of the MST
    for (int count = 0; count < V - 1; count++) 
    {
        int u = minKey(key, mstSet, V);
        mstSet[u] = true;

        // Update key and parent for adjacent vertices
        for (int v = 0; v < V; v++) {
            // Check if there is an edge between u and v in the original graph, 
            // if v is not in the MST, and if the weight is less than the current weight for v
            if (graph.adjList[u][v] && !mstSet[v] && graph.adjList[u][v] < key[v]) 
            {
                parent[v] = u; // Update the parent of v to u
                key[v] = graph.adjList[u][v]; // Update the weight of the edge to v
            }
        }
    }

    printMST(parent, graph);
}
