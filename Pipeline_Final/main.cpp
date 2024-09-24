

#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include "ClientConnectionStage.hpp"
#include "CommunicationStage.hpp"
#include "CommandExecuteStage.hpp"
#include "DisconnecterStage.hpp"
#define PORT 9034

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

#include <iostream>
#include <thread>

// Other includes as necessary...

#include <iostream>
#include <thread>

// Other includes as necessary...

int main() {
    int server_fd = setup_server();

    // Create instances of each stage
    ClientConnectionStage clientConnectionStage(server_fd);
    CommunicationStage communicationStage;
    CommandExecuteStage commandExecuteStage;
    DisconnecterStage disconnecterStage;

    // Main loop for handling the pipeline
    while (true) {
        // Enqueue client connection
        clientConnectionStage.enqueue([&clientConnectionStage, &communicationStage, &disconnecterStage, &commandExecuteStage]() {
            int client_fd = clientConnectionStage.StartConnectingClients();
            if (client_fd != -1) {
                // Enqueue command processing for the connected client
                communicationStage.enqueue([client_fd, &communicationStage, &disconnecterStage, &commandExecuteStage]() {
                    while (true) {
                        Command command = communicationStage.processClient(client_fd); // Get command from client

                        if (command == Command::Exit) {
                            disconnecterStage.disconnectClient(client_fd); // Disconnect the client
                            break; // Exit inner loop for this client
                        }

                        // Enqueue command execution
                        commandExecuteStage.enqueue([client_fd, command, &commandExecuteStage]() {
                            commandExecuteStage.processCommand(client_fd, command);
                        });
                    }
                });
            }
        });
    }

    return 0;
}


