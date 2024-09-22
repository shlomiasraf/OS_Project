#ifndef THREADPOOL_HPP
#define THREADPOOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <iostream>
class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads);
    ~ThreadPool();

    // Enqueue a new task
    template <typename F>
    void enqueue(F&& f);

    void start();
    void stop();

private:
    // Worker threads
    std::vector<std::thread> workers;

    // Queue for tasks
    std::queue<std::function<void()>> tasks;

    // Synchronization primitives
    std::mutex queueMutex;
    std::condition_variable condition;

    // To manage stopping the thread pool
    bool stopFlag;

    // Leader-follower mechanics
    int leaderIndex;              // Index of the current leader
    std::vector<bool> followers;  // Keep track of which threads are followers

    void threadFunction(int index);    // Function executed by threads (both leader/follower)
    void becomeLeader(int index);      // Function to handle leader behavior
    void becomeFollower(int index);    // Function to handle follower behavior
};



template <typename F>
void ThreadPool::enqueue(F&& f) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
         std::cout << "Task enqueued by thread " << std::this_thread::get_id() << std::endl; // Log task enqueue
        tasks.emplace(std::forward<F>(f)); // Add the task to the queue
    }
    condition.notify_one(); // Notify one waiting thread
}

#endif // THREADPOOL_HPP

