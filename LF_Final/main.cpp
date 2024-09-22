#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "HandleSet.hpp"
#include "RequestHandling.hpp"
#include "Graph.hpp"
#include <unordered_set>
#include "ThreadPool.hpp" // Include your ThreadPool header
#define PORT 9034 // Port number for the server

Graph graph(0, 0);

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

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
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

int setup_client_connection(int server_fd) {
    // Accept a new client connection
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        std::cerr << "Failed to accept client" << std::endl;
        return -1;
    }
    return client_fd;
}


int main() {
    int server_fd = setup_server();
    HandleSet handleSet;
    handleSet.addHandle(server_fd); // Add server socket to HandleSet
    RequestHandling requestHandler(graph); // Pass the graph to RequestHandling
    ThreadPool pool(6); // Initialize the ThreadPool with 6 threads
    std::cout << "Server listening on port " << PORT << std::endl;

    std::unordered_set<int> processedHandles; // Set to track processed handles

    while (true) {
        int readyHandle = handleSet.waitForEvent(); 
        if (readyHandle < 0) {
            std::cerr << "Error waiting for event" << std::endl;
            continue;
        }

        // Check if the ready handle is the server socket
        if (readyHandle == server_fd) {
            // Accept a new client connection
            int client_fd = setup_client_connection(server_fd);
            if (client_fd >= 0) {
                std::cout << "New client connected: " << client_fd << std::endl;
                handleSet.addHandle(client_fd); // Add client socket to HandleSet
            }
        } else {
            // Process client requests using the thread pool
            if (processedHandles.find(readyHandle) == processedHandles.end()) {
                processedHandles.insert(readyHandle); // Mark the handle as processed
                pool.enqueue([readyHandle, &requestHandler, &processedHandles]() {
                    std::cout << "Processing request from handle: " << readyHandle << std::endl;
                    requestHandler.processClient(readyHandle);
                });
            } else {
                std::cout << "Handle " << readyHandle << " has already been processed." << std::endl;
            }
        }
    }

    // Cleanup (not reached in this example)
    close(server_fd);
    return 0;
}



