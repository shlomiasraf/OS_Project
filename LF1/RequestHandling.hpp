#ifndef REQUESTHANDLING_HPP
#define REQUESTHANDLING_HPP
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <mutex>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>
#include <queue>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <set>
#include <cstring> 
#include "Graph.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "ThreadPool.hpp"

// Enum for commands
enum class Command {
    Newgraph,
    Prim,
    Kruskal,
    Addedge,
    Removeedge,
    Exit,
    Invalid
};

class RequestHandling {
public:
    // Constructor
    RequestHandling(Graph& graph);

    // Destructor
    //~RequestHandling();
    // Helper function to process each client
    void processClient(int clientfd);

private:
    Graph graph;  // Shared graph object
    void Newgraph(int clientfd);
    void Addedge(int clientfd);
    void RemoveEdge(int clientfd);
    void getMSTAlgorithm(Command type, int clientfd);
    // Function to get the command from the string input
    Command getCommandFromString(const std::string& commandStr);
    void processCommand(int clientfd, Command command);


};

#endif // REQUESTHANDLING_HPP
