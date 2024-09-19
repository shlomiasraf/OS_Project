#ifndef ACTIVEOBJECT_HPP
#define ACTIVEOBJECT_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <algorithm>
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include "primMST.hpp"
#include "kruskalMST.hpp"
#include "Graph.hpp"

enum class Command {
    Newgraph,
    Prim,
    Kruskal,
    Addedge,
    Removeedge,
    Exit,
    Invalid
};
// Abstract base class for active objects
class ActiveObject {
public:
    // Constructor
    ActiveObject();

    // Destructor
    virtual ~ActiveObject();

    // Enqueue a method request to be processed
    virtual void enqueue(std::function<void()> request) = 0;

    // Start processing requests
    void start();

    // Stop processing requests
    void stop();

protected:
    // Process requests (to be implemented by derived classes)
    virtual void execute() = 0;

private:
    // The worker thread for processing requests
    std::thread workerThread;

    // Queue for method requests
    std::queue<std::function<void()>> requestQueue;

    // Mutex for synchronizing access to the queue
    std::mutex queueMutex;

    // Condition variable for waiting for requests
    std::condition_variable queueCondition;

    // Flag to indicate whether to stop processing
    bool shouldStop;

    void workerFunction();
};

#endif // ACTIVEOBJECT_HPP
