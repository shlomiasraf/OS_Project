#include "Graph.hpp"
#include <sstream>  // for std::stringstream

// Create a new graph with V vertices
Graph::Graph(int V, int edges) : V(V), edges(edges), adjList(V, std::vector<int>(V, 0)) {}

Graph::Graph(){}

void Graph::resize(int size)
{
    adjList.resize(size, std::vector<int>(size, 0));  
}
// Add an edge to the graph
void Graph::addEdge(int src, int dest, int weight) 
{
    if(adjList[src][dest] == 0 && weight > 0)
    {
        edges++;
    }
    adjList[src][dest] = weight;
    adjList[dest][src] = weight;  // For undirected graph
}
// Remove an edge from the graph
void Graph::removeEdge(int src, int dest) 
{
    if(adjList[src][dest] > 0)
    {
        edges--;
    }
    // Set the edge weights between src and dest to 0
    adjList[src][dest] = 0;
    adjList[dest][src] = 0;  // For undirected graph
}
// Serialize the graph data into a string
std::string Graph::serialize() const 
{
    std::stringstream ss;
    ss << V << " " << edges << "\n";
    for (int i = 0; i < V; ++i) 
    {
        for (int j = 0; j < V; ++j) 
        {
            if (adjList[i][j] != 0) 
            {
                ss << i+1 << " " << j+1 << " " << adjList[i][j] << "\n";  // Add each edge
            }
        }
    }
    return ss.str();
}

// Deserialize a graph from a string
Graph Graph::deserialize(const std::string& data) 
{
    std::stringstream ss(data);
    int V, edges;
    ss >> V >> edges;
    Graph graph(V, edges);

    int src, dest, weight;
    while (ss >> src >> dest >> weight) 
    {
        graph.addEdge(src-1, dest-1, weight);
    }

    return graph;
}
