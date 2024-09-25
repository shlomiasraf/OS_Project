#ifndef GLOBALGRAPH_HPP
#define GLOBALGRAPH_HPP

#include "Graph.hpp"

class GlobalGraph {
public:
    // Get the singleton instance
    static GlobalGraph& getInstance() {
        static GlobalGraph instance; // Guaranteed to be destroyed.
        return instance;
    }
    void setGraph(const Graph& newGraph) { graph = newGraph; }	
    // Delete copy constructor and assignment operator
    GlobalGraph(const GlobalGraph&) = delete;
    GlobalGraph& operator=(const GlobalGraph&) = delete;

    // Accessor for the graph
    Graph& getGraph() { return graph; }
	
private:
    GlobalGraph() : graph(0, 0) {} // Initialize with default values
    Graph graph; // The global graph instance
};

#endif // GLOBALGRAPH_HPP

