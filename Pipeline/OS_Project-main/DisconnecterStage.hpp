#ifndef DISCONNECTERSTAGE_HPP
#define DISCONNECTERSTAGE_HPP

#include "ActiveObject.hpp"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <queue>
#include <functional>

class DisconnecterStage : public ActiveObject
{
private:
    std::mutex mtx; // Mutex to protect disconnect process
    std::condition_variable cv; // Condition variable to signal readiness
    std::queue<std::function<void()>> requestQueue; // Queue for client disconnection requests
    bool clientDisconnected = false; // Flag to indicate if the client has disconnected
    bool shutdown = false; // Flag to stop the execution thread
    std::thread workerThread; // Thread to execute requests

    // Thread worker function to process requests
    void processRequests();

public:
    DisconnecterStage();
    ~DisconnecterStage();

    void enqueue(std::function<void()> request) override;
    void execute() override;

    // Simulate client disconnection
    void disconnectClient(int clientId);


    // Shutdown the stage gracefully
    void shutdownStage();
};

#endif // DISCONNECTERSTAGE_HPP

