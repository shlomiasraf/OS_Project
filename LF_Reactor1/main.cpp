#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>           // Include thread support
#include "reactor.hpp"    
#include "RequestHandling.hpp"
#include "Graph.hpp"
#include "ThreadPool.hpp"  
#include <csignal> // Include for signal handling

#define PORT 9034          
volatile sig_atomic_t isShuttingDown = 0; // Flag for graceful shutdown
Graph graph(0, 0);         
ThreadPool* poolPtr;       
RequestHandling* requestHandlerPtr; 
Reactor* reactorPtr;       

void signalHandler(int signum) {
    if (!isShuttingDown) { // Check if not already shutting down
        std::cout << "Received signal " << signum << ". Shutting down..." << std::endl;
        isShuttingDown = 1; // Set the shutdown flag
    }
}

void handleClientRequests(int client_fd) {
    while (true) {
        {

            if (isShuttingDown) {
                break; // Exit if shutting down
            }
        }
        char buf[1024];
        int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
        if (nbytes <= 0) {
            std::cerr << "Client disconnected or error receiving data from client." << std::endl;
            close(client_fd);
            return;  // Exit the function on error or disconnect
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
        });
    }
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

// Function to set up the server
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

// Function to handle new connections in the reactor
void* newConnectionHandler(int fd) {
    while (true) {
        int client_fd = setup_client_connection(fd);
        if (client_fd >= 0) {
            std::cout << "New client connected: " << client_fd << std::endl;

            // Create a new thread to handle requests from this client
            std::thread clientThread(handleClientRequests, client_fd);
            clientThread.detach();  // Detach the thread to allow it to run independently
        }
    }
    return nullptr;
}

int main() {
    signal(SIGINT, signalHandler); // Register signal handler for Ctrl+C

    int server_fd = setup_server();
    Reactor* reactor = (Reactor*)startReactor();    
    reactorPtr = reactor;                           
    RequestHandling requestHandler(graph);          
    requestHandlerPtr = &requestHandler;            
    ThreadPool pool(6);                             
    poolPtr = &pool;                               

    std::cout << "Server listening on port " << PORT << std::endl;

    // Add the server_fd to the reactor using a function pointer
    if (addFdToReactor(reactor, server_fd, newConnectionHandler) < 0) {
        std::cerr << "Failed to add server_fd to reactor" << std::endl;
        return -1;
    }

   while (true) {
    {
        if (isShuttingDown) {
            break; // Exit the loop if shutting down
        }
    }
    runReactor(reactor); // Process events in the reactor
}

    // Perform cleanup on shutdown
    std::cout << "Cleaning up resources..." << std::endl;
    stopReactor(reactor); // Stop the reactor
    poolPtr->shutdown();   // Call the shutdown method of the thread pool
    close(server_fd);      // Close the server socket

    return 0;
}

