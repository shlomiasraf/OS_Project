#ifndef COMMANDEXECUTESTAGE_HPP
#define COMMANDEXECUTESTAGE_HPP

#include "ActiveObject.hpp"
#include "kruskalMST.hpp"
#include "primMST.hpp"
#include "Graph.hpp"
#include "GlobalGraph.hpp"
class CommandExecuteStage : public ActiveObject {
public:
    CommandExecuteStage();
    ~CommandExecuteStage();
    Command processCommand(int client_fd, Command command);
private:
    void AddEdge(int client_fd);
    void RemoveEdge(int client_fd);
    void Newgraph(int client_fd);
    void getMSTAlgorithm(Command type, int client_fd);
};

#endif // COMMANDEXECUTESTAGE_HPP

