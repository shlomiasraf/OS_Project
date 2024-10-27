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
