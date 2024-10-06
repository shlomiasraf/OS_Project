#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include <iostream>
#include <poll.h>  // Include the necessary header for poll()
#include <unistd.h> // Include for close()
#include <atomic>   // For std::atomic
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

// Main server loop already handles clients through polling

int main() {
    int server_fd = setup_server();
    std::cout << "Server listening on port " << PORT << std::endl;

    ClientConnectionStage clientConnectionStage(server_fd);
    CommunicationStage communicationStage;
    DisconnecterStage disconnecterStage;

    std::vector<pollfd> fds;

    // Add the server file descriptor to the pollfd array
    pollfd server_pollfd = {};
    server_pollfd.fd = server_fd;
    server_pollfd.events = POLLIN; // Monitor server socket for incoming connections
    fds.push_back(server_pollfd);

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

    const int timeout_ms = 1000; // 1 second timeout for poll

    // Accept and manage multiple clients
    while (!shutdown_flag.load()) 
    {
        // Use poll to check for new connections or data from clients
        int poll_count = poll(fds.data(), fds.size(), timeout_ms);

        if (poll_count == -1) {
            perror("poll"); // Handle error during poll
            break;
        }

        // Iterate over the pollfd array to check for events
        for (size_t i = 0; i < fds.size(); ++i) {
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server_fd) {
                    // Server socket ready to accept new connections
                    int client_fd = clientConnectionStage.StartConnectingClients();

                    if (client_fd >= 0) {
                        std::cout << "Accepted new client: " << client_fd << std::endl;

                        // Add the new client socket to the pollfd array
                        pollfd client_pollfd = {};
                        client_pollfd.fd = client_fd;
                        client_pollfd.events = POLLIN; // Monitor client socket for incoming data
                        fds.push_back(client_pollfd);
                    } else {
                        std::cerr << "Failed to accept client connection." << std::endl;
                    }
                } else {
                    // Data available on a client socket
                    int client_fd = fds[i].fd;

                    // Peek at the socket to check if there is data to be read
                    char buffer[1];
                    int peek_result = recv(client_fd, buffer, sizeof(buffer), MSG_PEEK);

                    if (peek_result > 0) {
                        // There is data available, process the command from the client
                        Command command = communicationStage.enqueueProcessClient(client_fd);

                        // Check if the command is 'Exit' or if shutdown is triggered
                        if (command == Command::Exit || shutdown_flag.load()) {
                            std::cout << "Disconnecting client: " << client_fd << std::endl;
                            disconnecterStage.disconnectClient(client_fd);
                            close(client_fd);  // Ensure the socket is closed

                            // Remove the client socket from the pollfd array
                            fds.erase(fds.begin() + i);
                            --i;  // Adjust index after erasing
                        }
                    } else if (peek_result == 0) {
                        // Connection was closed by the client
                        std::cout << "Client disconnected: " << client_fd << std::endl;
                        disconnecterStage.disconnectClient(client_fd);
                        close(client_fd);

                        // Remove the client socket from the pollfd array
                        fds.erase(fds.begin() + i);
                        --i;  // Adjust index after erasing
                    } else {
                        // Error occurred
                        std::cerr << "Error peeking data on client socket: " << client_fd << std::endl;
                    }
                }
            }
        }
    }

    // Close server socket, no new clients are accepted after shutdown
    close(server_fd);
    std::cout << "Server stopped accepting new clients." << std::endl;

    // Wait for shutdown thread to finish
    if (shutdown_thread.joinable()) {
        shutdown_thread.join();
    }

    std::cout << "Server has shut down." << std::endl;
    return 0;
}

