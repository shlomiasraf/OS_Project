#ifndef COMMUNICATIONSTAGE_HPP
#define COMMUNICATIONSTAGE_HPP

#include "ActiveObject.hpp"
#include "kruskalMST.hpp"
#include "primMST.hpp"
#include "Graph.hpp"
#include "GlobalGraph.hpp"
#include <future>

class CommunicationStage : public ActiveObject {
public:
    CommunicationStage();
    ~CommunicationStage();
    Command enqueueProcessClient(int client_fd);
private:

    void processCommand(int client_fd, Command command);
    std::string handleReceiveData(int client_fd);
    Command getCommandFromString(const std::string& commandStr);
    void AddEdge(int client_fd);
    void RemoveEdge(int client_fd);
    void Newgraph(int client_fd);
    void getMSTAlgorithm(Command type, int client_fd);
};
#endif
