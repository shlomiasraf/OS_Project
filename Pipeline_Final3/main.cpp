#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <atomic>  // For shutdown flag
#include <mutex>
#include "ClientConnectionStage.hpp"
#include "CommunicationStage.hpp"
#include "DisconnecterStage.hpp"

#define PORT 9034
const int MAX_CLIENTS = 10; // Maximum number of clients to accept

std::atomic<bool> shutdown_flag(false); // Global shutdown flag

// Helper function to set up the server
int setup_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

// Function to handle clients through all stages
void handleClient(int client_fd, CommunicationStage& communicationStage, DisconnecterStage& disconnecterStage) {
    std::cout << "Handling client " << client_fd << std::endl;
    while (!shutdown_flag.load()) {  // Check shutdown flag in loop
        // Process the command from the client
        Command command = communicationStage.enqueueProcessClient(client_fd);

        // Check if the command is 'Exit' or if shutdown is triggered
        if (command == Command::Exit || shutdown_flag.load()) {
            std::cout << "Disconnecting client: " << client_fd << std::endl;
            disconnecterStage.disconnectClient(client_fd);
            close(client_fd);  // Ensusre the socket is closed
            break;  // Exit the loop to stop the thread
        }
    }
    std::cout << "Finished handling client " << client_fd << std::endl;
}

int main() {
    int server_fd = setup_server();
    std::cout << "Server listening on port " << PORT << std::endl;

    ClientConnectionStage clientConnectionStage(server_fd);
    CommunicationStage communicationStage;
    DisconnecterStage disconnecterStage;

    std::vector<std::thread> clientThreads;

    // Server thread to handle input for shutdown
    std::thread shutdown_thread([&]() {
        std::string input;
        while (true) {
            std::cout << "Type 'shutdown' to stop the server: ";
            std::cin >> input;
            if (input == "shutdown") {
                std::cout << "'shutdown' command received" << std::endl;
                shutdown_flag.store(true);  // Set shutdown flag
                break;
            }
        }
    });

    // Accept and manage multiple clients
    while (!shutdown_flag.load()) {
        int client_fd = clientConnectionStage.StartConnectingClients();

        if (client_fd < 0) {
            if (shutdown_flag.load()) break; // Exit if shutdown is triggered
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }
        std::cout << "Accepted new client: " << client_fd << std::endl;

        clientThreads.emplace_back(handleClient, client_fd, std::ref(communicationStage), std::ref(disconnecterStage));
    }

    // Close server socket, no new clients are accepted after shutdown
    close(server_fd);
    std::cout << "Server stopped accepting new clients." << std::endl;

    // Join all client threads to ensure they finish before shutdown
    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();  // Wait for each thread to finish
        }
    }

    // Wait for shutdown thread to finish
    if (shutdown_thread.joinable()) {
        shutdown_thread.join();
    }

    std::cout << "Server has shut down." << std::endl;
    return 0;
}

