#include "ActiveObject.hpp"

// Constructor
ActiveObject::ActiveObject() : shouldStop(false) {}

// Destructor
ActiveObject::~ActiveObject() {
    stop(); // Ensure the worker thread is stopped on destruction
}

void ActiveObject::start() {
    shouldStop = false;
    workerThread = std::thread(&ActiveObject::workerFunction, this); // Create thread
    std::cout << "Thread created for " << typeid(*this).name() << " with ID: " 
              << workerThread.get_id() << std::endl; // Log thread ID and type
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
        } // The lock is released here

        // Execute the task inside the lock to block other tasks
        if (task) {
            // Lock the queue again before executing the task
            {
                std::unique_lock<std::mutex> lock(queueMutex); // Re-locking

                // Log which thread is executing the task
                std::cout << "Thread ID: " << std::this_thread::get_id() << " is executing task." << std::endl;
                task();  // Execute the task
            } // The lock will be released here
        }
    }
}



// Enqueue method
void ActiveObject::enqueue(std::function<void()> newTask) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        requestQueue.push(std::move(newTask));
    }
    queueCondition.notify_one(); // Notify one waiting thread
}



