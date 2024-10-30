#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <mutex>
#include <iostream>
#include <poll.h>  
#include <unistd.h> 
#include <atomic>  
#include <string.h>
#include "CommandExecute.hpp"
#define PORT 9034

std::mutex shutdown_mutex;
std::atomic<bool> shutdown_flag(false); // Global shutdown flag

int setup_client_connection(int server_fd) 
{
    // Accept a new client connection
    int client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0; // Indicate no client connected
        } else {
            std::cerr << "Failed to accept client: " << strerror(errno) << std::endl;
            return -1; // Return -1 for other errors
        }
    }
    return client_fd; // Return the client file descriptor if successful
}

Command getCommandFromString(const std::string& commandStr) {
    std::string lowerCommand = commandStr;
    if (!lowerCommand.empty() && lowerCommand.back() == '\n') {
        lowerCommand.pop_back();
    }
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);

    if (lowerCommand == "newgraph") {
        return Command::Newgraph;
    } else if (lowerCommand == "prim") {
        return Command::Prim;
    } else if (lowerCommand == "kruskal") {
        return Command::Kruskal;
    } else if (lowerCommand == "addedge") {
        return Command::Addedge;
    } else if (lowerCommand == "removeedge") {
        return Command::Removeedge;
    } else if (lowerCommand == "exit") {
        return Command::Exit;
    }
    return Command::Invalid;
}

int setup_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set the SO_REUSEADDR option to reuse the port
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "setsockopt(SO_REUSEADDR) failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << strerror(errno) << std::endl;
        close(server_fd);  // Ensure we close the socket on failure
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed: " << strerror(errno) << std::endl;
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    return server_fd;
}

void handle_client(int client_fd) {
    CommandExecute commandExecutor;
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer)); // Clear the buffer
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            if (bytes_read == 0) {
//                std::cout << "Client " << client_fd << " disconnected." << std::endl;
            } else {
                std::cerr << "Error reading from client " << client_fd << ": " << strerror(errno) << std::endl;
            }
            break; // Exit the loop if client disconnects or an error occurs
        }
        Command command=getCommandFromString(buffer);
        commandExecutor.processCommand(client_fd,command);
        if(command==Command::Exit){
          break;
        }
    }
    close(client_fd); // Close the client socket
}
int main() {
    int server_fd = setup_server();
    std::vector<std::thread> client_threads;
    // Server thread to handle input for shutdown
    std::thread shutdown_thread([&]() {
        std::string input;
        while (true) {
            std::cin >> input;
            if (input == "shutdown") {
                std::lock_guard<std::mutex> lock(shutdown_mutex); // Lock while setting the flag
                shutdown_flag.store(true);  // Set shutdown flag
                break;
            }
        }
    });

    while (true) {
        // Poll for incoming connections and check for shutdown
        struct pollfd fds[1];
        fds[0].fd = server_fd;
        fds[0].events = POLLIN; // We're interested in input events (new connections)

        int poll_result = poll(fds, 1, 1000); // 1-second timeout for polling

        {
            std::lock_guard<std::mutex> lock(shutdown_mutex); // Lock to check shutdown flag
            if (shutdown_flag.load()) {
                break; // Exit if shutdown flag is set
            }
        }

        if (poll_result == -1) {
            std::cerr << "Poll error: " << strerror(errno) << std::endl;
            break;
        } else if (poll_result == 0) {
            continue; // Timeout, no new connection
        }

        if (fds[0].revents & POLLIN) {
            // Accept and manage multiple clients
            int client_fd = setup_client_connection(server_fd);

            if (client_fd >= 0) {
            client_threads.emplace_back(handle_client, client_fd); // Store the thread                client_thread.detach(); // Detach the thread to allow it to run independently
         
            }
        }
    }
    
    LFCompute& LF = LFCompute::getInstance();
    LF.cleanup();
    // Close server socket and stop accepting new clients
    close(server_fd);

    // After all clients are processed, join the threads before exiting
    for (auto& t : client_threads) {
    if (t.joinable()) {
        t.join(); // Wait for each thread to finish
      }
    }
    // Wait for shutdown thread to finish
    if (shutdown_thread.joinable()) {
        shutdown_thread.join();
    }

    return 0;
}