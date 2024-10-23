#include "ThreadPool.hpp"
#include <iostream>
#include <functional>

ThreadPool::ThreadPool(size_t numThreads) : leaderIndex(0), stop(false) {
    initializeWorkers(numThreads);
}

void ThreadPool::initializeWorkers(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this, i] { workerThread(i); });
    }
}

void ThreadPool::workerThread(size_t i) {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] {
                return stop || !tasks.empty();
            });

            if (stop && tasks.empty()) return; // Proper exit condition
            // No task to process
            if (tasks.empty()) continue; // Retry loop if no tasks are available
            // Only the current leader processes the task
            if (i == leaderIndex) {
                task = std::move(tasks.front());
                tasks.pop();
            }
        }

        // Process the task outside of the lock
        if (task) {
            std::cout << "Leader Index: " << leaderIndex << 
                ", Thread ID: " << std::this_thread::get_id() 
                << " is executing task." << std::endl;
            leaderIndex = (leaderIndex + 1) % workers.size(); 
            task(); // Execute the task
        }
    }
}



// Destructor for automatic shutdown
ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
        
    condition.notify_all(); // Wake up all threads to stop
    }
    // Join all threads after stopping the task queue
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::move(task)); // Add the task to the queue
    
    condition.notify_one(); // Notify one worker thread to wake up
    }
}
