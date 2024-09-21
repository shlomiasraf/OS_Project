#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads) : stopFlag(false), activeLeaderIndex(0) {
    workers.reserve(numThreads);
}

ThreadPool::~ThreadPool() {
    stop();
}

void ThreadPool::start() {
    for (size_t i = 0; i < workers.capacity(); ++i) {
        workers.emplace_back([this, i] {
            while (true) {
                std::function<void()> task;

                // Lock the task queue
                {
                    std::unique_lock<std::mutex> lock(tasksMutex);

                    // Wait for tasks or stopping signal
                    condition.wait(lock, [this] {
                        return stopFlag || !tasks.empty();
                    });

                    if (stopFlag && tasks.empty()) {
                        return; // Exit the thread
                    }

                    // If this thread is the leader, pick a task
                    if (i == activeLeaderIndex) {
                        if (!tasks.empty()) {
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                    }
                }

                // Execute the task
                if (task) {
                    printf("Executing task...\n");  // Debug line
                    task();

                    // After executing, pass leadership
                    {
                        std::unique_lock<std::mutex> lock(tasksMutex);
                        activeLeaderIndex = (activeLeaderIndex + 1) % workers.size();
                    }

                    // Notify all threads to let the new leader take over
                    condition.notify_all();
                }
            }
        });
    }
}





void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        tasks.push(std::move(task));
    }
    condition.notify_all();
}

void ThreadPool::stop() {
    {
        std::unique_lock<std::mutex> lock(tasksMutex);
        stopFlag = true;
    }
    condition.notify_all();

    // Join all threads
    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}
