// ThreadPool.hpp
#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
    void shutdown();
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    size_t leaderIndex; // Change to size_t
};

#endif // THREADPOOL_HPP

