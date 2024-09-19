#include "ThreadPool.hpp"

ThreadPool::ThreadPool(int numThreads) : stop(false) 
{
    for (int i = 0; i < numThreads; ++i) {
        workers.emplace_back([this, i] { this->threadTask(i); });
    }
}

ThreadPool::~ThreadPool() 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread &worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueueTask(const std::function<void()>& task) 
{
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void ThreadPool::threadTask(int threadID) 
{
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !tasks.empty() || stop; });
            if (stop && tasks.empty())
                return;
            task = std::move(tasks.front());
            tasks.pop();
        }
        task(); // Execute the task
    }
}
