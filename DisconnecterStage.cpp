#include "DisconnecterStage.hpp"

// Simulate client disconnection
void DisconnecterStage::disconnectClient(int clientId) {
    std::unique_lock<std::mutex> lock(mtx);  // Lock for disconnect operation
    cv.wait(lock, [this] { return !clientDisconnected; });  // Wait if the client is already being disconnected

    // Mark the client as being disconnected
    clientDisconnected = true;

    // Critical section: Perform the disconnection
    std::cout << "Disconnecting client: " << clientId << std::endl;

    // Simulate some disconnection process
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // After disconnection, mark client as disconnected and notify waiting threads
    clientDisconnected = false;
    cv.notify_all();  // Notify other threads waiting for disconnection

    std::cout << "Client " << clientId << " has been safely disconnected." << std::endl;
    close(clientId);
}

// Example: Handle multiple clients disconnecting
void DisconnecterStage::handleMultipleClients(std::vector<int> clientIds) {
    for (int clientId : clientIds) {
        // Simulate a client disconnection in the disconnect stage
        std::thread(&DisconnecterStage::disconnectClient, this, clientId).detach();
    }
}
