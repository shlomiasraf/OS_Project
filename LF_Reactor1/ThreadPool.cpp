#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads) : leaderIndex(0), stop(false) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this, i] {
            while (true) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });

                    if (this->stop && this->tasks.empty())
                        return;

                    // Only the current leader processes the task
                    if (i == leaderIndex && !this->tasks.empty()) {
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                }

                // If a task was processed, rotate the leader
                if (task) {
                    task(); // Execute the task
                    leaderIndex = (leaderIndex + 1) % workers.size(); // Rotate the leader
                }
            }
        });
    }
}

// Destructor for automatic shutdown
ThreadPool::~ThreadPool() {
    shutdown(); // Call the shutdown method for controlled shutdown
}

// Graceful shutdown method
void ThreadPool::shutdown() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true; // Set the stop flag
    }
    condition.notify_all(); // Wake up all threads to stop

    // Join all threads
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
    }
    condition.notify_all(); // Notify one worker thread to wake up
}

