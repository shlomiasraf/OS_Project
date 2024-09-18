#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include "CommunicationStage.hpp"
#include "CommandExecuteStage.hpp"
#include "ActiveObject.hpp"

// Function to set up the server
int setup_server() {
    int server_fd;
    struct sockaddr_in address;
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(9034);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        return -1;
    }
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", 9034);
    return server_fd;
}

// Function to accept a new client connection
int setup_client_connection(int server_fd) {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof(remoteaddr);
    char remoteIP[INET6_ADDRSTRLEN];
    int new_fd;

    new_fd = accept(server_fd, (struct sockaddr*)&remoteaddr, &addrlen);
    if (new_fd == -1) {
        perror("accept");
        return -1;
    }

    if (remoteaddr.ss_family == AF_INET) {
        struct sockaddr_in* s = (struct sockaddr_in*)&remoteaddr;
        inet_ntop(AF_INET, &s->sin_addr, remoteIP, sizeof(remoteIP));
    } else {
        struct sockaddr_in6* s = (struct sockaddr_in6*)&remoteaddr;
        inet_ntop(AF_INET6, &s->sin6_addr, remoteIP, sizeof(remoteIP));
    }
    printf("New connection from %s on socket %d\n", remoteIP, new_fd);

    return new_fd;
}

// Pipeline function to handle client connections and commands
void pipeline() {
    CommunicationStage communicationStage;
    CommandExecuteStage commandExecuteStage;

    // Initialize the server
    int server_fd = setup_server();
    if (server_fd == -1) {
        return;  // Server setup failed
    }

    // Create threads for the stages
    std::thread communicationThread([&]() {
        communicationStage.execute();  // Start handling client communication
    });

    std::thread commandThread([&]() {
        commandExecuteStage.execute();  // Start processing commands
    });

    while (true) {
        // Accept a new client connection
        int client_fd = setup_client_connection(server_fd);
        if (client_fd == -1) {
            continue;  // If accepting connection failed, continue to the next iteration
        }
        
        // Create a thread to handle each client separately
        std::thread([client_fd, &communicationStage, &commandExecuteStage]() {
            while (true) {
                // Receive data from the client
                std::string receivedData = communicationStage.handleReceiveData(client_fd);
                // Convert received data to command, stage 1 input->output
                Command command = communicationStage.getCommandFromString(receivedData);
		
                // Enqueue the command for execution, stage 2 input(stage 1 input)->output
                commandExecuteStage.enqueue([client_fd, command, &commandExecuteStage]() {
                    commandExecuteStage.processCommand(client_fd, command);
                });
                
            }
            close(client_fd);  // Close the client connection after the client exits
        }).detach();  // Detach the thread to run independently
    }

    close(server_fd);  // Close the server when finished

    // Join threads (if they ever need to terminate, which they won't in this infinite loop)
    communicationThread.join();
    commandThread.join();
}

int main() {
    // Run the pipeline function to handle clients
    pipeline();
    return 0;
}
