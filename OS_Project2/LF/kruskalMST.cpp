
#include "kruskalMST.hpp"


// Function to perform Kruskal's algorithm
std::vector<std::vector<std::pair<int, int>>> KruskalMST::FindMST(Graph &graph) {
    int V = graph.V; 
    std::vector<std::vector<int>> edgeList;  // Store edges in {src, dest, weight} format

    // Step 1: Extract edges from the adjacency matrix or list
    for (int u = 0; u < V; u++) {
        for (int v = u + 1; v < V; v++) {
            if (graph.adjList[u][v] != 0) {  // Adjust based on adjacency matrix or list
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
        if (e >= V - 1) break;  // Stop if MST has V-1 edges

        int u = edge[0];
        int v = edge[1];
        int weight = edge[2];

        int setU = MSTInfo::find(parent, u);
        int setV = MSTInfo::find(parent, v);

        // If including this edge doesn't cause a cycle
        if (setU != setV) {
            result.push_back({u, v, weight});
            e++;
            MSTInfo::Union(parent, rank, setU, setV);
        }
    }

    // Convert result to adjacency list format for return
    std::vector<std::vector<std::pair<int, int>>> mstAdjList(V);  // No need for static

    for (const auto& edge : result) {
        int u = edge[0];
        int v = edge[1];
        int weight = edge[2];
        mstAdjList[u].emplace_back(v, weight);
        mstAdjList[v].emplace_back(u, weight);  // Since the graph is undirected
    }
    return mstAdjList;  // Return by value, modern C++ will optimize this
}
