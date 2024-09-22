#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "reactor.hpp"    // Include the reactor header
#include "RequestHandling.hpp"
#include "Graph.hpp"
#include "ThreadPool.hpp"  // Include the ThreadPool header
#define PORT 9034          // Port number for the server

Graph graph(0, 0);         // Initialize your graph
ThreadPool* poolPtr;       // Pointer to the thread pool
RequestHandling* requestHandlerPtr; // Pointer to the request handler
Reactor* reactorPtr;       // Pointer to the reactor

void* handleNewConnection(int client_fd) {
    // Read the command from the client
    char buf[1024];
    int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (nbytes <= 0) {
        std::cerr << "Error receiving data from client." << std::endl;
        close(client_fd);
        return nullptr;
    }
    buf[nbytes] = '\0'; // Null-terminate the string

    std::string commandStr(buf);
    commandStr.erase(commandStr.find_last_not_of(" \n\r\t") + 1); // Trim whitespace

    // Parse the command from the string
    Command command = requestHandlerPtr->getCommandFromString(commandStr);

    // Enqueue the command processing
    poolPtr->enqueue([client_fd, command]() {
        std::cout << "Processing command from client: " << client_fd << std::endl;
        
        // Call the request handler to process the command
        requestHandlerPtr->processCommand(client_fd, command);
        close(client_fd); // Close client connection after processing
    });

    return nullptr; // Return immediately to avoid blocking
}


// Function to set up the server
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

// Function to set up client connection
int setup_client_connection(int server_fd) {
    // Accept a new client connection
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        std::cerr << "Failed to accept client" << std::endl;
        return -1;
    }
    return client_fd;
}

// Function to handle new connections in the reactor
void* newConnectionHandler(int fd) {
    int client_fd = setup_client_connection(fd);
    if (client_fd >= 0) {
        std::cout << "New client connected: " << client_fd << std::endl;
        return handleNewConnection(client_fd); // Handle the new connection
    }
    return nullptr;
}

int main() {
    int server_fd = setup_server();
    Reactor* reactor = (Reactor*)startReactor();    // Start the reactor
    reactorPtr = reactor;                           // Set global reactor pointer
    RequestHandling requestHandler(graph);          // Initialize the request handler
    requestHandlerPtr = &requestHandler;            // Set global request handler pointer
    ThreadPool pool(6);                             // Initialize the thread pool with 6 threads
    poolPtr = &pool;                                // Set global thread pool pointer

    std::cout << "Server listening on port " << PORT << std::endl;

    // Add the server_fd to the reactor using a function pointer
    if (addFdToReactor(reactor, server_fd, newConnectionHandler) < 0) {
        std::cerr << "Failed to add server_fd to reactor" << std::endl;
        return -1;
    }

    // Reactor loop
    while (true) {
        runReactor(reactor); // Process events in the reactor
    }

    // Cleanup (not reached in this example)
    stopReactor(reactor);
    close(server_fd);
    return 0;
}
