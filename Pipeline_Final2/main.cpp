#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "ClientConnectionStage.hpp"
#include "CommunicationStage.hpp"
#include "DisconnecterStage.hpp"
#include <mutex>

#define PORT 9034
const int MAX_CLIENTS = 10; // Maximum number of clients to accept

// Helper function to set up the server
int setup_server() {
    // Create a socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the port
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening for incoming connections
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

// Function to handle clients through all stages
void handleClient(int client_fd, CommunicationStage& communicationStage, DisconnecterStage& disconnecterStage) {
    while (true) {
       // Process the command
      Command command = communicationStage.enqueueProcessClient(client_fd);
        // If the command is Exit, disconnect the client
        if (command == Command::Exit) {
            disconnecterStage.disconnectClient(client_fd);
            break;
        }
    }
}

int main() {
    // Create server socket
    int server_fd = setup_server();
    std::cout << "Server listening on port " << PORT << std::endl;

    // Create stages
    ClientConnectionStage clientConnectionStage(server_fd);
    CommunicationStage communicationStage;
    DisconnecterStage disconnecterStage;
    // Vector to hold client threads
    std::vector<std::thread> clientThreads;

    // Accept and manage multiple clients
    while (true) {
        // Handle client connection through ClientConnectionStage
        int client_fd = clientConnectionStage.StartConnectingClients();

        if (client_fd < 0) {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue; // Move on to the next iteration
        }
        std::cout << "Accepted new client: " << client_fd << std::endl;
        // Start a new thread to handle the client through all stages
    clientThreads.emplace_back(handleClient, client_fd, std::ref(communicationStage),std::ref(disconnecterStage));	
     }

    // Cleanup (never reached in this example)
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    close(server_fd);
    return 0;
}

