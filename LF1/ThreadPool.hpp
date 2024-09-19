#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Enqueue a new task
    void enqueue(std::function<void()> task);

    // Start the thread pool
    void start();

    // Stop the thread pool
    void stop();

private:
    std::vector<std::thread> workers; // Worker threads
    std::queue<std::function<void()>> tasks; // Task queue
    std::mutex tasksMutex; // Mutex for protecting task queue
    std::condition_variable condition; // Condition variable to notify workers
    bool stopFlag; // Flag to indicate stopping
    size_t activeLeaderIndex; // Track the current leader thread
};

