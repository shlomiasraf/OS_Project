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
#include <sys/socket.h>
enum class Command {
    Newgraph,
    Prim,
    Kruskal,
    Addedge,
    Removeedge,
    Exit,
    Invalid
};

class ThreadPool {
public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();

    void enqueue(std::function<void()> task);
    void shutdown();
    
private:
    void initializeWorkers(size_t numThreads);
    void workerThread(size_t i);

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable leaderCondition;
    std::atomic_bool stop;
};


#endif // THREADPOOL_HPP

