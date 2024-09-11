// Client.cpp
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "Graph.hpp"
#define PORT 8080

class Client {
public:
    void startClient();
};

void Client::startClient() {
  int sock = 0;
    struct sockaddr_in serv_addr;
    int vertex, edges;

    std::cout << "Please enter number of vertices and edges: ";
    std::cin >> vertex >> edges;

    // Create a Graph object with the given number of vertices
    Graph graph(vertex,edges);

    std::cout << "Please enter the edges (format: u v weight for each edge): " << std::endl;
    for (int i = 0; i < edges; i++) 
    {
        int u, v, weight;
        std::cin >> u >> v >> weight;
        while(u > vertex || v > vertex)
        {
            std::cout << "Please enter valid edges: " << std::endl;
            std::cin >> u >> v >> weight;
        }
        graph.addEdge(u-1, v-1, weight);  // Add the edge to the graph
    }


    // Serialize the graph and send it to the server
    std::string serializedGraph = graph.serialize();
    const char* graphData = serializedGraph.c_str();  // Convert to C-style string

    char algorithmChoice[10];
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error\n";
        return;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address\n";
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed\n";
        return;
    }

    // Send graph data to server
    send(sock, graphData, strlen(graphData), 0);
    std::cout << "Graph data sent\n";

    // Receive prompt from server
    read(sock, buffer, 1024);
    std::cout << "Server: " << buffer;

    // Get algorithm choice
    std::cout << "Enter algorithm choice (PRIM or KRUSKAL): ";
    std::cin >> algorithmChoice;

    // Send algorithm choice to server
    send(sock, algorithmChoice, strlen(algorithmChoice), 0);
    std::cout << "Algorithm choice sent\n";

    // Receive response
    memset(buffer, 0, sizeof(buffer));  // Clear the buffer again
    read(sock, buffer, 1024);
    std::cout << "Server response: " << buffer << "\n";

    close(sock);
}

int main() {
    Client client;
    client.startClient();
    return 0;
}
