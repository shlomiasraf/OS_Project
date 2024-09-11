// Graph.hpp
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
        Graph(int V,int edges);
        void addEdge(int src, int dest, int weight);
        std::string serialize() const;  // Add serialize function
        static Graph deserialize(const std::string& data);  // For deserialization

};

#endif
