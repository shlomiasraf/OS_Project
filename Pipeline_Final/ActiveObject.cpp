#include "ActiveObject.hpp"

// Constructor
ActiveObject::ActiveObject() : shouldStop(false) {}

// Destructor
ActiveObject::~ActiveObject() {
    stop(); // Ensure the worker thread is stopped on destruction
}

// Start the worker thread
void ActiveObject::start() {
    shouldStop = false; // Make sure the stop flag is reset
    workerThread = std::thread(&ActiveObject::workerFunction, this); // Launch the worker thread
}

// Stop the worker thread
void ActiveObject::stop() {
    {
        std::lock_guard<std::mutex> lock(queueMutex); // Lock the queue mutex
        shouldStop = true; // Set stop flag to true
    }
    queueCondition.notify_all(); // Notify the worker thread to stop waiting
    if (workerThread.joinable()) {
        workerThread.join(); // Wait for the worker thread to finish
    }
}

// Worker function for the thread
void ActiveObject::workerFunction() {
    while (!shouldStop) {
        std::function<void()> task;

        // Lock the queue for safe access
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            // Wait for a task or stop signal
            queueCondition.wait(lock, [&]() { return !requestQueue.empty() || shouldStop; });

            // If stopping and no more tasks, exit the loop
            if (shouldStop && requestQueue.empty()) {
                return;
            }

            // Retrieve the next task
            task = std::move(requestQueue.front());
            requestQueue.pop();
        }

        // Execute the task
        if (task) {
            task();
        }
    }
}

