#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <vector>
#include <string>

class Graph 
{
    public:
        int V;  // Number of vertices
        int edges;
        std::vector<std::vector<int>> adjList;  // Adjacency matrix for the graph
        Graph(int V, int edges);
        Graph();
        void addEdge(int src, int dest, int weight);
        void removeEdge(int src, int dest);
        void resize(int size);
};

#endif
