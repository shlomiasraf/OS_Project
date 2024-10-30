#include "ActiveObject.hpp"

ActiveObject::ActiveObject() : stopRequested_(false) {}

ActiveObject::~ActiveObject() {
    stop(); // Ensure that the worker thread is stopped in the destructor
}

void ActiveObject::start() {
    worker_ = std::thread(&ActiveObject::process, this);
}

void ActiveObject::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_); // Protect the task queue
        stopRequested_ = true; // Set stop request flag
        condition_.notify_all(); // Notify the worker thread to wake up if waiting
    }
    if (worker_.joinable()) {
        worker_.join(); // Wait for the worker thread to finish
    }
}

void ActiveObject::enqueue(std::function<void()> task) {
    {
        std::lock_guard<std::mutex> lock(mutex_); // Protect the task queue
        tasks_.push(std::move(task)); // Add the task to the queue
        condition_.notify_one(); // Notify the worker thread that a task is available
}
}


void ActiveObject::process() {
    while (true) {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(mutex_);
            // Wait until there's a task or a stop request
            condition_.wait(lock, [this] {
                return stopRequested_ || !tasks_.empty();
            });

            if (stopRequested_ && tasks_.empty()) {
                return; // Exit if stop is requested and no tasks are left
            }

            // Get the task from the queue
            task = std::move(tasks_.front());
            tasks_.pop();
        }

            task(); // Execute the task outside of the lock
    }
}
