#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t numThreads) : stopFlag(false), leaderIndex(0) {
    followers.resize(numThreads, true); // Initially all threads are followers
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back(&ThreadPool::threadFunction, this, i); // Pass the index to each thread
    }
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    stopFlag = false;
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stopFlag = true;
    }
    condition.notify_all(); // Wake up all threads to stop them
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();  // Wait for each thread to finish
        }
    }
}

void ThreadPool::threadFunction(int index) {
    while (true) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this, index] { 
                return stopFlag || (index == leaderIndex && !tasks.empty()); 
            });

            if (stopFlag && tasks.empty()) return; // Exit if stop and no tasks

            if (index == leaderIndex) {
                becomeLeader(index); // Current thread becomes leader
            } else {
                becomeFollower(index); // Current thread remains a follower
            }
        }
    }
}

void ThreadPool::becomeLeader(int index) {
    // Fetch and execute task
    if (!tasks.empty()) {
        std::function<void()> task = std::move(tasks.front());
        tasks.pop();

        // Log the task being picked by leader
        std::cout << "Leader thread (index " << index << ", ID " << std::this_thread::get_id() 
                  << ") is handling the task." << std::endl;

        // Execute task
        task(); // Call the task
    }

    // After handling the task, assign next leader
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        leaderIndex = (leaderIndex + 1) % workers.size();  // Pass leadership to next thread
    }
    condition.notify_all();  // Notify the next leader to take over
}

void ThreadPool::becomeFollower(int index) {
    // This function doesn't need to do anything specific for followers
    std::cout << "Follower thread (index " << index << ") is waiting." << std::endl;
}

