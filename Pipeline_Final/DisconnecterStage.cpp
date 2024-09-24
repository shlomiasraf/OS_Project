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

// Override the enqueue function from ActiveObject
void DisconnecterStage::enqueue(std::function<void()> request) {
    // Lock the queue to ensure thread-safe access
    std::lock_guard<std::mutex> lock(queueMutex);
    
    // Add the request to the queue
    requestQueue.push(request);
    
    // Notify the worker thread that a new request is available
    queueCondition.notify_one();
}

// Method to disconnect the client
void DisconnecterStage::disconnectClient(int client_fd) {
        // Perform the client disconnection
        std::cout << "Disconnecting client: " << client_fd << std::endl;
        // Add your actual disconnect logic here, e.g., closing the socket
        close(client_fd);

}



