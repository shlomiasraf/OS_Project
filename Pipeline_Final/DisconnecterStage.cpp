#include "DisconnecterStage.hpp"
#include <iostream>

// Constructor
DisconnecterStage::DisconnecterStage() {
    // Start the worker thread to process requests
    start();
}

// Destructor
DisconnecterStage::~DisconnecterStage() {
    // Ensure the worker thread is stopped before destruction
    stop();
}


// Method to disconnect the client
void DisconnecterStage::disconnectClient(int client_fd) {
          // Enqueue the task to disconnect the client asynchronously
    enqueue([this, client_fd]() {
        // Perform the client disconnection
        std::cout << "Disconnecting client: " << client_fd << std::endl;       
        // Add your actual disconnect logic here, e.g., closing the socket
        close(client_fd);
    });
}



