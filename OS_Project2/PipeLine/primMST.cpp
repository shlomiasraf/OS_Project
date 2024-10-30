
#include "primMST.hpp"

// Prim's algorithm for constructing the MST
std::vector<std::vector<std::pair<int, int>>> PrimMST::FindMST(Graph &graph) 
{
    int V = graph.V;
    
    if (V == 0) {
        std::cerr << "Error: Graph is empty!" << std::endl;
        static std::vector<std::vector<std::pair<int, int>>> emptyMST;
        return emptyMST;
    }

    std::vector<int> parent(V);  // Array to store the constructed MST
    std::vector<int> key(V, INF); // Key values used to pick minimum weight edge
    std::vector<bool> mstSet(V, false); // To represent the set of vertices included in MST
    key[0] = 0; // Start with the first vertex
    parent[0] = -1; // The first node is always the root of the MST

    for (int count = 0; count < V - 1; count++) 
    {
        int u = MSTInfo::minKey(key, mstSet, V); // Pick the minimum key vertex
        mstSet[u] = true;

        // Update key and parent for adjacent vertices
        for (int v = 0; v < V; v++) {
            // Check if there is an edge between u and v in the original graph, 
            // if v is not in the MST, and if the weight is less than the current key value for v
            if (graph.adjList[u][v] && !mstSet[v] && graph.adjList[u][v] < key[v]) 
            {
                parent[v] = u; // Update the parent of v to u
                key[v] = graph.adjList[u][v]; // Update the key to reflect the new minimum weight
            }
        }
    }

    std::vector<std::vector<std::pair<int, int>>> mstAdjList(V);
    for (int i = 1; i < V; i++) {
        int u = parent[i];
        if (u != -1) { // Ensure a valid parent before adding to the MST
            int weight = graph.adjList[u][i];
            mstAdjList[u].emplace_back(i, weight);
            mstAdjList[i].emplace_back(u, weight);  // Since the graph is undirected
        }
    }
    return mstAdjList;
}