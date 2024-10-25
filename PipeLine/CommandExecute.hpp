#ifndef COMMANDEXECUTESTAGE_HPP
#define COMMANDEXECUTESTAGE_HPP

#include "ActiveObject.hpp"
#include "kruskalMST.hpp"
#include "primMST.hpp"
#include "Graph.hpp"
#include "Pipeline.hpp"
class CommandExecute {
public:
     Command processCommand(int client_fd, Command command);
    Pipeline& pipe = Pipeline::getInstance();
private:
    Graph graph;
        std::mutex resultMutex; // Mutex to protect access to the graph
    void AddEdge(int client_fd);
    void RemoveEdge(int client_fd);
    void Newgraph(int client_fd);
    void getMSTAlgorithm(Command type, int client_fd);
};

#endif // COMMANDEXECUTESTAGE_HPP

