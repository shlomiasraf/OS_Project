#ifndef REQUESTHANDLING_HPP
#define REQUESTHANDLING_HPP

#include <string>
#include <mutex>
#include <sys/socket.h>
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
    RequestHandling(int numThreads);

    // Destructor
    ~RequestHandling();

    // Function to get the command from the string input
    Command getCommandFromString(const std::string& commandStr);
    void processCommand(int clientfd, Command command);

    // Main processing function
    void monitorHandles();

private:
    Graph graph;  // Shared graph object
    std::mutex graphMutex;  // Mutex to protect shared graph
    ThreadPool threadPool;  // Thread pool for managing tasks
    std::set<int> client_fds; // HandleSet (client file descriptors)
    std::mutex clientMutex;  // Mutex to protect client file descriptors

    void Newgraph(int clientfd);
    void Addedge(int clientfd);
    void RemoveEdge(int clientfd);
    void getMSTAlgorithm(Command type, int clientfd);

    // Helper function to process each client
    void processClient(int clientfd);
};

#endif // REQUESTHANDLING_HPP
