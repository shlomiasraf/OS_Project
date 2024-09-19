#include "ActiveObject.hpp"

ActiveObject::ActiveObject():shouldStop(false){}

ActiveObject::~ActiveObject()
{
    stop();
}

void ActiveObject::start()
{
    workerThread= std::thread(&ActiveObject::workerThread,this);
}

void ActiveObject::stop()
{
     {
        std::unique_lock<std::mutex> lock(queueMutex);
        shouldStop = true;
    }
    queueCondition.notify_all(); // Wake up the thread to stop processing
    if (workerThread.joinable()) {
        workerThread.join(); // Wait for the worker thread to finish
    }
}

// Add a method request to the queue
void ActiveObject::enqueue(std::function<void()> request) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        requestQueue.push(request);
    }
    queueCondition.notify_one(); // Notify the worker thread of the new request
}

// Process requests
void ActiveObject::workerFunction() {
    while (true) {
        std::function<void()> request;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCondition.wait(lock, [this]() { return !requestQueue.empty() || shouldStop; });
            if (shouldStop && requestQueue.empty()) {
                return; // Exit the thread if stopping and no more requests
            }
            request = requestQueue.front();
            requestQueue.pop();
        }
        request(); // Execute the request outside of the lock
    }
}
