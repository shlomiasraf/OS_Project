// DisconnecterStage.hpp
#include "ActiveObject.hpp"

#ifndef DISCONNECTERSTAGE_HPP
#define DISCONNECTERSTAGE_HPP

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

class DisconnecterStage 
{
    private:
        std::mutex mtx;                      // Mutex to protect disconnect process
        std::condition_variable cv;        // Condition variable to signal readiness
        bool clientDisconnected = false;   // Flag to indicate if the client has disconnected

    public:
        // Simulate client disconnection
        void disconnectClient(int clientId);

        // Example: Handle multiple clients disconnecting
        void handleMultipleClients(std::vector<int> clientIds);
};

#endif // DISCONNECTERSTAGE_HPP
