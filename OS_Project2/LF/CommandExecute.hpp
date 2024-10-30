#ifndef COMMANDEXECUTESTAGE_HPP
#define COMMANDEXECUTESTAGE_HPP

#include "kruskalMST.hpp"
#include "primMST.hpp"
#include "Graph.hpp"
#include "ThreadPool.hpp"
#include "LFCompute.hpp"
#include "MSTFactory.hpp"

class CommandExecute {
public:
     Command processCommand(int client_fd, Command command);

private:
    Graph graph;
    LFCompute& LF = LFCompute::getInstance();
    std::mutex resultMutex; // Mutex to protect access to the graph
    void AddEdge(int client_fd);
    void RemoveEdge(int client_fd);
    void Newgraph(int client_fd);
    void getMSTAlgorithm(Command type, int client_fd);
};

#endif // COMMANDEXECUTESTAGE_HPP

