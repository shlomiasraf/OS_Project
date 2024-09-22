#ifndef REQUESTHANDLING_HPP
#define REQUESTHANDLING_HPP

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string>
#include <mutex>
#include <netinet/in.h>
#include <cerrno>
#include <queue>
#include <functional>
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include <sstream>
#include <set>
#include <cstring>
#include "Graph.hpp"
#include "primMST.hpp"
#include "kruskalMST.hpp"


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
    // Existing constructor
    RequestHandling(Graph& graph);
    
    // Destructor
    //~RequestHandling();
    // Helper function to process each client

    void processCommand(int clientfd, Command command);
    Command getCommandFromString(const std::string& commandStr);
private:
    Graph graph;  // Shared graph object
    void Newgraph(int clientfd);
    void Addedge(int clientfd);
    void RemoveEdge(int clientfd);
    void getMSTAlgorithm(Command type, int clientfd);
	
    std::string receiveFullInput(int clientfd);
};

#endif // REQUESTHANDLING_HPP

