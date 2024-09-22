#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t numThreads) : leaderIndex(0), stop(false) { // Reorder initialization
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

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.emplace(std::move(task));
    }
    condition.notify_all(); // Notify all workers to wake up
}

