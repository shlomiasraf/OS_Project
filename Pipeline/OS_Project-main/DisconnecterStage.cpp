#include "DisconnecterStage.hpp"

// Constructor starts the worker thread
DisconnecterStage::DisconnecterStage() {
    workerThread = std::thread(&DisconnecterStage::processRequests, this);
}

// Destructor ensures that the worker thread is properly terminated
DisconnecterStage::~DisconnecterStage() {
    shutdownStage();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

// Enqueue a new request (e.g., client disconnection)
void DisconnecterStage::enqueue(std::function<void()> request) {
    std::unique_lock<std::mutex> lock(mtx);
    requestQueue.push(request);
    cv.notify_one(); // Signal the worker thread that a new request is available
}

// Execute the requests (process requests sequentially)
void DisconnecterStage::execute() {
    std::function<void()> request;
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return !requestQueue.empty() || shutdown; });
        
        if (shutdown && requestQueue.empty()) return; // Exit if shutting down

        request = requestQueue.front();
        requestQueue.pop();
    }
    request(); // Execute the request
}

// Simulate disconnecting a client
void DisconnecterStage::disconnectClient(int clientId) {
    std::cout << "Disconnecting client with ID: " << clientId << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate delay in disconnection
    std::cout << "Client " << clientId << " disconnected successfully." << std::endl;
}

// Gracefully shut down the stage
void DisconnecterStage::shutdownStage() {
    {
        std::unique_lock<std::mutex> lock(mtx);
        shutdown = true;
    }
    cv.notify_one(); // Wake up the worker thread to finish execution
}

// Worker thread to process requests from the queue
void DisconnecterStage::processRequests() {
    while (!shutdown) {
        execute(); // Process queued requests
    }
}

