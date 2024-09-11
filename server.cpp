// Server.cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Graph.hpp"
#include "MSTFactory.hpp"

#define PORT 8080

class Server {
public:
    void startServer();
};

void Server::startServer() 
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) 
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) 
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server is listening on port " << PORT << "...\n";

    // Accept incoming connections
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) 
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive graph data
    ssize_t bytesRead = read(new_socket, buffer, 1024);
    if (bytesRead < 0) 
    {
        perror("Read failed");
        exit(EXIT_FAILURE);
    }
    buffer[bytesRead] = '\0';
    std::cout << "Received graph data: " << "\n" << buffer;

    // Parse graph data and create graph
    std::string graphData(buffer);
    Graph graph = Graph::deserialize(graphData);  // Use deserialize here

    // Receive algorithm choice
    const char* askAlgoMsg = "Which algorithm do you want to use? (PRIM/KRUSKAL)\n";
    send(new_socket, askAlgoMsg, strlen(askAlgoMsg), 0);

    bytesRead = read(new_socket, buffer, 1024);
    buffer[bytesRead] = '\0';
    std::cout << "Received algorithm choice: " << buffer << "\n";

    // Determine the algorithm
    AlgorithmType algoType = (strncmp(buffer, "PRIM", 4) == 0) ? AlgorithmType::PRIM : AlgorithmType::KRUSKAL;

    // Compute the MST
    auto mstAlgorithm = MSTFactory::getMSTAlgorithm(algoType);
    if (mstAlgorithm) 
    {
        mstAlgorithm(graph);
        
    } 
    else 
    {
        std::cout << "Invalid algorithm type.\n";
    }

    // Send response
    const char* response = "MST Computed";
    send(new_socket, response, strlen(response), 0);
    close(new_socket);
}

int main() {
    Server server;
    server.startServer();
    return 0;
}
